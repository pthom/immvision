#include "immdebug/immdebug_internal.h"

#include <ios>
#include <fstream>
#include <filesystem>
#include <random>
#include <iostream>

namespace ImmVision
{
    namespace ImmDebug_Internal
    {
        template<typename T>
        void WriteValue(std::ostream &os, T v)
        {
            os.write((char *) &v, sizeof(T));
        }

        template<typename T>
        T ReadValue(std::istream &is)
        {
            T v;
            is.read((char *) &v, sizeof(T));
            return v;
        }

        void WriteImageBuffer(std::ostream &os, const ImageBuffer &img)
        {
            WriteValue<int>(os, img.width);
            WriteValue<int>(os, img.height);
            WriteValue<int>(os, img.channels);
            WriteValue<int>(os, static_cast<int>(img.depth));
            WriteValue<size_t>(os, img.step);

            // Write pixel data row by row (source may have non-contiguous stride)
            size_t row_bytes = (size_t)img.width * img.channels * ImageDepthSize(img.depth);
            for (int y = 0; y < img.height; y++)
            {
                const uint8_t* row = static_cast<const uint8_t*>(img.data) + y * img.step;
                os.write(reinterpret_cast<const char*>(row), (std::streamsize)row_bytes);
            }
        }

        ImageBuffer ReadImageBuffer(std::istream &is)
        {
            int width = ReadValue<int>(is);
            int height = ReadValue<int>(is);
            int channels = ReadValue<int>(is);
            ImageDepth depth = static_cast<ImageDepth>(ReadValue<int>(is));
            size_t saved_step = ReadValue<size_t>(is);
            (void)saved_step;

            ImageBuffer img = ImageBuffer::Zeros(width, height, channels, depth);
            size_t row_bytes = (size_t)width * channels * ImageDepthSize(depth);
            for (int y = 0; y < height; y++)
            {
                uint8_t* row = static_cast<uint8_t*>(img.data) + y * img.step;
                is.read(reinterpret_cast<char*>(row), (std::streamsize)row_bytes);
            }
            return img;
        }

        void WriteString(std::ostream &os, const std::string &v)
        {
            WriteValue(os, v.size());
            os.write(v.data(), std::streamsize(sizeof(char) * v.size()));
        }

        std::string ReadString(std::istream &is)
        {
            std::size_t len = ReadValue<std::size_t>(is);
            char *buffer = new char[len + 1];
            is.read(buffer, std::streamsize(len));
            buffer[len] = '\0';
            std::string s(buffer);
            delete[]buffer;
            return s;
        }

        void WriteImagePayload(std::ostream &os, const ImagePayload &v)
        {
            WriteString(os, v.Legend);
            WriteValue<Point2d>(os, v.ZoomCenter);
            WriteValue<double>(os, v.ZoomRatio);
            WriteString(os, v.ZoomKey);
            WriteString(os, v.ColorAdjustmentsKey);
            WriteValue<bool>(os, v.isColorOrderBGR);
            WriteImageBuffer(os, v.Image);
        }

        ImagePayload ReadImagePayload(std::istream &is)
        {
            ImagePayload r;
            r.Legend = ReadString(is);
            r.ZoomCenter = ReadValue<Point2d>(is);
            r.ZoomRatio = ReadValue<double>(is);
            r.ZoomKey = ReadString(is);
            r.ColorAdjustmentsKey = ReadString(is);
            r.isColorOrderBGR = ReadValue<bool>(is);
            r.Image = ReadImageBuffer(is);
            return r;
        }



        std::string ImmDebugFolder(bool whileWriting)
        {
            static auto temp_path = std::filesystem::temp_directory_path();

            std::string folder;
            if (whileWriting)
                folder = temp_path.string() + "/ImmDebug/Writing";
            else
                folder = temp_path.string() + "/ImmDebug";
            if (! std::filesystem::is_directory(folder))
                std::filesystem::create_directories(folder);
            return folder;
        }

        std::string MakeUniqueFilename(bool whileWriting)
        {
            static long long id = []() {
                std::default_random_engine generator;
                generator.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
                std::uniform_int_distribution<long long> distribution(100000000000, 1000000000000);
                long long id_ = distribution(generator);
                return id_;
            }();

            id++;
            return ImmDebugFolder(whileWriting) + "/" + std::to_string(id) + ".bindata";
        }

        bool RemoveOneFile(const std::filesystem::path& path)
        {
            std::error_code ec;
            bool successRemove = std::filesystem::remove(path, ec);
            if (!successRemove)
            {
                std::cerr << "Error while removing " << path.string() << std::endl;
                std::cerr << ec.message() << std::endl;
                return false;
            }
            return true;
        }

        void RemoveOldImages(double maxAge_Seconds = 3600)
        {
            namespace fs = std::filesystem;

            auto now = fs::file_time_type::clock::now();

            std::string folder = ImmDebugFolder(false);
            for(const fs::directory_entry& entry : fs::recursive_directory_iterator(folder))
            {
                try
                {
                    const auto& path = entry.path();
                    if (! fs::is_regular_file(path))
                        continue;

                    std::chrono::duration<double> fileAge_DurationSeconds = now - fs::last_write_time(path);
                    double fileAge_Seconds = fileAge_DurationSeconds.count();

                    if (fileAge_Seconds > maxAge_Seconds)
                        RemoveOneFile(path);
                }
                catch (fs::filesystem_error & e)
                {
                    std::cerr << e.what();
                }
            }
        }


        void SaveImagePayload(const ImagePayload & imagePayload)
        {
            RemoveOldImages();
            std::string filename_whileWriting = MakeUniqueFilename(true);
            std::ofstream ofs(filename_whileWriting, std::ios::binary);
            WriteImagePayload(ofs, imagePayload);
            ofs.close();
            std::string filename_Done = MakeUniqueFilename(false);
            rename(filename_whileWriting.c_str(), filename_Done.c_str());
        }


        std::optional<ImagePayload> ReadImagePayload()
        {
            RemoveOldImages();
            std::optional<ImagePayload> imagePayload;

            std::string folder = ImmDebugFolder(false);
            namespace fs = std::filesystem;

            std::vector<fs::path> imagePayloads;
            for(const fs::directory_entry& entry : fs::directory_iterator(folder))
            {
                const auto& path = entry.path();
                if (! fs::is_regular_file(path))
                    continue;
                imagePayloads.push_back(path);
            }

            if (!imagePayloads.empty())
            {
                if (imagePayloads.size() > 1)
                {
                    std::sort(imagePayloads.begin(), imagePayloads.end(),
                              [](const fs::path& f1, const fs::path& f2) {
                                  return last_write_time(f1) < last_write_time(f2);
                              }
                    );
                }
                auto oldestPayload = imagePayloads.front();
                std::ifstream is(oldestPayload, std::ios::binary);
                imagePayload = ReadImagePayload(is);
                is.close();
                RemoveOneFile(oldestPayload);
                return imagePayload;
            }


            return std::nullopt;
        }


    } // namespace ImmDebug_Internal
} // namespace ImmVision
