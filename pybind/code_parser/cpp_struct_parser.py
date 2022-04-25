import os
import typing
from dataclasses import dataclass

from prettyprinter import pprint
import prettyprinter

import code_utils

prettyprinter.install_extras(exclude=["django", "ipython"])


THIS_DIR = os.path.dirname(__file__)
IMMVISION_CPP_FOLDER = f"{THIS_DIR}/../../src/immvision"
IMMVISION_CPP_PYBIND_FOLDER = f"{THIS_DIR}/../src_cpp"


@dataclass
class StructCode:
    struct_name: str = ""
    line_start: int = 0
    line_end: int = 0
    body_code: str = ""


@dataclass
class StructAttribute:
    name_cpp: str = ""
    name_python: str = ""
    type: str = ""
    default_value: str = ""
    comment: str = ""
    line_number: int = 0  # from the body_code line_start


@dataclass
class CodeRegionComment:
    comment: str = ""
    line_number: int = 0


@dataclass
class Variant_Attribute_CodeRegion:
    line_number: int = 0
    code_region_comment: CodeRegionComment = None
    struct_attribute: StructAttribute = None


def _extract_struct_code(code: str, struct_name: str) -> StructCode:
    """
    Very dumb code parser
    :return:
    * an extract of the code, which is the body of the struct
    * the line number where it starts
    """
    r = StructCode()
    r.struct_name = struct_name

    code_lines = code.split("\n")

    struct_lines = []
    in_struct = False
    nb_accolades = 0
    for line_number, code_line in enumerate(code_lines):
        if f"struct {struct_name}" in code_line:
            in_struct = True
            r.line_start = line_number

        if "{" in code_line and in_struct:
            nb_accolades += 1

        if "}" in code_line and in_struct:
            nb_accolades -= 1
            if nb_accolades == 0:
                if in_struct:
                    r.line_end = line_number
                in_struct = False

        if nb_accolades > 0 and in_struct:
            struct_lines.append(code_line)

    if struct_lines[0].strip() == "{":
        struct_lines = struct_lines[1:]

    r.body_code = "\n".join(struct_lines)
    return r


def extract_struct_attributes(struct_code: StructCode) -> typing.List[StructAttribute]:
    """
    *Very* dumb parser, that works with our simple structs
    """
    code_lines = struct_code.body_code.split("\n")
    code_lines = map(lambda s: s.strip(), code_lines)
    numbered_code_lines = list(enumerate(code_lines))

    def is_attribute_line(nb_and_code_line) -> bool:
        code_line = nb_and_code_line[1]
        if code_line.startswith("//") or len(code_line) == 0:
            return False
        return True

    def parse_attribute(nb_and_code_line) -> StructAttribute:
        """
        Can handle lines like
            ColorAdjustmentsValues ColorAdjustments = ColorAdjustmentsValues();
        or
            std::vector <cv::Point> WatchedPixels;
        """
        line_number = nb_and_code_line[0]
        code_line = nb_and_code_line[1]

        r = StructAttribute()
        r.line_number = line_number
        code_line = code_line.strip()
        if code_line[-1] == ";":
            code_line = code_line[:-1]
        if "=" in code_line:
            equal_sides = code_line.split("=")
            r.default_value = equal_sides[1].strip()
            code_line = equal_sides[0].strip()
        if " " not in code_line:
            raise RuntimeError(f"can't parse attribute at line {code_line}")
        last_space_pos = code_line.rindex(" ")
        r.type = code_line[:last_space_pos].strip()
        r.name_cpp = code_line[last_space_pos:].strip()
        r.name_python = code_utils.to_snake_case(r.name_cpp)
        return r

    def add_attribute_comment(attribute: StructAttribute) -> str:
        # Search for comment beginning with // immediately on top of the attribute
        comment_lines_before_attribute_decl = []
        line_number = attribute.line_number - 1
        while line_number > 0:
            previous_code_line: str = numbered_code_lines[line_number][1]
            if not previous_code_line.startswith("//"):
                break
            previous_code_line = previous_code_line[2:].strip()
            if len(previous_code_line) == 0:
                break
            comment_lines_before_attribute_decl.append(previous_code_line)
            line_number = line_number - 1
        comment_lines_before_attribute_decl = list(reversed(comment_lines_before_attribute_decl))
        attribute.comment = "\n".join(comment_lines_before_attribute_decl)
        # if no comment found on top, try to find one to the right? (YAGNI, this is a *simple* parser)

    attribute_lines = list(filter(is_attribute_line, numbered_code_lines))
    all_attributes = list(map(parse_attribute, attribute_lines))
    for attribute in all_attributes:
        add_attribute_comment(attribute)
    return all_attributes


def extract_code_region_comments(struct_code: StructCode) -> typing.List[CodeRegionComment]:
    """
    Code region comments look like this
    //
    // This is a code region comment.
    // It can have several lines, but needs to have an empty comment before and after
    //
    """
    code_lines = struct_code.body_code.split("\n")
    code_lines = map(lambda s: s.strip(), code_lines)
    numbered_code_lines = list(enumerate(code_lines))

    all_code_region_comments = []

    def get_code_line(line_number: int) -> str:
        return numbered_code_lines[line_number][1]

    def is_empty_comment(line_number):
        return get_code_line(line_number).strip() == "//"

    def is_filled_comment(line_number):
        c = get_code_line(line_number)
        return c.strip().startswith("//") and len(c[2:].strip()) > 0

    def get_filled_comment(line_number):
        assert (is_filled_comment(line_number))
        c = get_code_line(line_number)
        return c.strip()[2:]

    def append_line(str1, str2):
        if len(str1) == 0:
            return str2
        else:
            return str1 + "\n" + str2

    current_code_region_comment = None
    for line_number, code_line in numbered_code_lines:
        if is_empty_comment(line_number) and current_code_region_comment is None:
            current_code_region_comment = CodeRegionComment()
            current_code_region_comment.line_number = line_number
        elif is_empty_comment(line_number) and current_code_region_comment is not None:
            all_code_region_comments.append(current_code_region_comment)
            current_code_region_comment = None
        elif is_filled_comment(line_number) and current_code_region_comment is not None:
            current_code_region_comment.comment = \
                append_line(current_code_region_comment.comment, get_filled_comment(line_number))

    return all_code_region_comments


def extract_struct_attributes_and_regions(struct_code: str) -> typing.List[Variant_Attribute_CodeRegion]:
    struct_attributes = extract_struct_attributes(struct_code)
    code_region_comments = extract_code_region_comments(struct_code)
    r = []
    for sa in struct_attributes:
        v = Variant_Attribute_CodeRegion()
        v.line_number = sa.line_number
        v.struct_attribute = sa
        r.append(v)
    for cr in code_region_comments:
        v = Variant_Attribute_CodeRegion()
        v.line_number = cr.line_number
        v.code_region_comment = cr
        r.append(v)

    r = sorted(r, key=lambda x: x.line_number)
    return r


def extract_struct_infos(code: str, struct_name: str) -> typing.List[Variant_Attribute_CodeRegion]:
    struct_code = _extract_struct_code(code, struct_name)
    infos = extract_struct_attributes_and_regions(struct_code)
    return infos


def make_struct_doc(struct_infos: typing.List[Variant_Attribute_CodeRegion]) -> str:
    doc = ""
    for info in struct_infos:
        if info.code_region_comment is not None:
            doc = doc + "\n" + info.code_region_comment.comment + "\n"
        elif info.struct_attribute is not None:
            attr = info.struct_attribute
            attr_doc = f"{attr.name_python}: {attr.type}"
            if len(attr.default_value) > 0:
                attr_doc = attr_doc + " = " + attr.default_value

            if len(attr.comment) > 0:
                comment_lines = attr.comment.split("\n")
                comment_lines = map(lambda l: "\t\t\t" + l, comment_lines)
                comment = "\n".join(comment_lines)
                attr_doc = attr_doc + "\n" + comment
            doc = doc + "\t* " + attr_doc + "\n"

    doc = code_utils.apply_opencv_replacements(doc)
    return doc


def make_struct_attributes_cpp_code(struct_name: str, struct_infos: typing.List[Variant_Attribute_CodeRegion]):
    r = ""

    def add_line(l):
        nonlocal r
        r = r + l + "\n"

    for info in struct_infos:
        if info.code_region_comment is not None:
            add_line("//")
            for line in info.code_region_comment.comment.split("\n"):
                add_line("// " + line)
            add_line("//")
        if info.struct_attribute is not None:
            #   .def_readwrite("ImageDisplaySize", &ImageNavigatorParams::ImageDisplaySize, "The Size")
            attr = info.struct_attribute
            line = f'.def_readwrite("{attr.name_python}", &{struct_name}::{attr.name_cpp}, "{attr.comment}")'
            add_line(line)
    return r


def write_struct_attributes_pybindcpp_code(
        struct_header_filename: str,
        struct_name: str,
        inout_pybindcpp_filename: str
    ):
    struct_infos = extract_struct_infos(
        code_utils.read_text_file(struct_header_filename),
        struct_name)
    code_marker = f"{struct_name}.attributes"
    code_utils.write_code_between_markers(
        inout_pybindcpp_filename,
        code_marker,
        make_struct_attributes_cpp_code(struct_name, struct_infos)
    )





def playground():
    with open(f"{IMMVISION_CPP_FOLDER}/image_navigator.h", "r") as f:
        code = f.read()

    struct_name = "ImageNavigatorParams"

    struct_infos = extract_struct_infos(code, struct_name)
    # pprint(struct_infos)

    # print(make_struct_doc(infos))
    #indent_size = 8
    #print(make_struct_attributes_cpp_code(struct_name, indent_size, infos))


    struct_header_filename = f"{IMMVISION_CPP_FOLDER}/image_navigator.h"
    struct_name = "ImageNavigatorParams"
    pybindcpp_filename = f"{IMMVISION_CPP_PYBIND_FOLDER}/pybind_image_navigator.cpp"
    write_struct_attributes_pybindcpp_code(
        struct_header_filename, struct_name, pybindcpp_filename)


if __name__ == "__main__":
    playground()
