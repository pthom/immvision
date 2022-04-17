#!/usr/bin/env python3

import sys
import subprocess
import os

INVOKE_DIR = os.getcwd()
THIS_DIR = os.path.dirname(os.path.realpath(__file__))
REPO_DIR = os.path.realpath(THIS_DIR + "/../..")
DOCKER_IMAGE_NAME = "immvision_docker_builder_image"
DOCKER_CONTAINER_NAME = "immvision_docker_builder"
SOURCES_MOUNT_DIR = "/dvp/sources"
VNC_PORT = 5900
ONLY_SHOW_COMMAND = False
BUILD_COMMANDS = """
            echo 'Making build dir in the container' &&\\
            cd /dvp &&\\
            mkdir -p build &&\\
            cd build &&\\
            echo 'Running cmake and build' &&\\
            ../sources/scripts/build_utilities.py run --build_python_bindings=True -run_build_all &&\\
            echo 'Deploying binaries to the host machine inside scripts/docker_clang_build/bin_docker/' &&\\
            cp -a bin/ ../sources/scripts/docker_clang_build/bin_docker &&\\
            cp -a _pybind_libs/ ../sources/scripts/docker_clang_build/bin_docker 
            """

CHDIR_LAST_DIRECTORY = INVOKE_DIR


def my_chdir(folder):
    global CHDIR_LAST_DIRECTORY
    os.chdir(folder)
    if ONLY_SHOW_COMMAND and folder != CHDIR_LAST_DIRECTORY:
        print(f"cd {folder}")
    CHDIR_LAST_DIRECTORY = folder


def run(cmd):
    if ONLY_SHOW_COMMAND:
        print(cmd)
    else:
        print("#####################################################")
        print("# Running shell command:")
        print(cmd)
        print("#####################################################")
        subprocess.check_call(cmd, shell=True)


def help():
    print(f"""
        Usage: {sys.argv[0]} -build_image|-create_container|-bash|-remove_container|-remove_image|-run [any command and args] [--show_command]
        
            {sys.argv[0]} -build_image 
        Will build the image (call this first). It will be called {DOCKER_IMAGE_NAME}

            {sys.argv[0]} -create_container 
        Will create a container called {DOCKER_CONTAINER_NAME} from this image, where the sources are mounted at {SOURCES_MOUNT_DIR}.
        This container will start in detached mode (-d). Call this after build_image. 
        
            {sys.argv[0]} -bash 
        Will log you into a bash session in the previously created container.

            {sys.argv[0]} -run [any command and args]
        Will start the container and run the commands given after "-run".
        For example, "{sys.argv[0]} -run ls -al" will list the files.  

            {sys.argv[0]} -build
        Will start the container and build the project using the following commands:
        {BUILD_COMMANDS}

            {sys.argv[0]} -remove_container 
        Will remove the container (you will lose all modifications in the Docker container)
        
            {sys.argv[0]} -remove_image 
        Will remove the image

            --show_command 
        Will not run the command, but show you its command line.
        """)


def help_vnc():
    msg = f"""
    Inside the container, you can start a vnc server by running

        /start_x_vnc.sh &

    You can then view the container by running a VNC viewer on your local machine
    and connect it to the address localhost:{VNC_PORT}
    
    Note: the screen is initially black. You need to run a graphic program
    (for example xterm, or /dvp/build/bin/inspector_demo) in order to see something.
    """
    lines = msg.split("\n")
    lines = map(lambda line: "# " + line, lines)
    msg = "\n".join(lines)
    print(msg)


def main():
    global ONLY_SHOW_COMMAND
    os.chdir(THIS_DIR)
    if len(sys.argv) < 2:
        help()
        return

    for arg in sys.argv:
        if arg.lower() == "--show_command":
            ONLY_SHOW_COMMAND = True

    arg1 = sys.argv[1].lower()
    if arg1 == "-build_image":
        my_chdir(THIS_DIR)
        run(f"docker build -t {DOCKER_IMAGE_NAME} .")
    elif arg1 == "-create_container":
        run(f"docker run --name {DOCKER_CONTAINER_NAME} -p {VNC_PORT}:{VNC_PORT} -it -d -v {REPO_DIR}:{SOURCES_MOUNT_DIR} {DOCKER_IMAGE_NAME}  /bin/bash")
    elif arg1 == "-bash":
        help_vnc()
        run(f"docker start {DOCKER_CONTAINER_NAME} && docker exec -it {DOCKER_CONTAINER_NAME} /bin/bash")
    elif arg1 == "-remove_container":
        run(f"docker stop {DOCKER_CONTAINER_NAME}")
        run(f"docker rm {DOCKER_CONTAINER_NAME}")
    elif arg1 == "-remove_image":
        run(f"docker rmi {DOCKER_IMAGE_NAME}")
    elif arg1 == "-run":
        command = " ".join(sys.argv[2:])
        run(f"docker start {DOCKER_CONTAINER_NAME} && docker exec -it {DOCKER_CONTAINER_NAME} {command}")
    elif arg1 == "-build":
        commands = f'/bin/bash -c "{BUILD_COMMANDS}"'
        run(f"docker start {DOCKER_CONTAINER_NAME} && docker exec -it {DOCKER_CONTAINER_NAME} {commands}")
    else:
        help()


if __name__ == "__main__":
    main()
