import os
import subprocess
from dataclasses import dataclass
from pathlib import Path


# Directory global variables
INVOKE_DIR = os.getcwd()
THIS_DIR = os.path.dirname(os.path.realpath(__file__))
REPO_DIR = os.path.realpath(THIS_DIR + "/..")
EXTERNAL_DIR = REPO_DIR + "/external"
HOME_FOLDER = Path.home()
CHDIR_LAST_DIRECTORY = INVOKE_DIR


@dataclass
class Options:
    only_echo_command: bool = False


OPTIONS = Options()


def has_program(program_name):
    if os.name == "nt":
        paths = os.environ["PATH"].split(";")
    else:
        paths = os.environ["PATH"].split(":")
    for path in paths:
        prog_path = f"{path}/{program_name}"
        if os.name == "nt":
            prog_path += ".exe"
        if os.path.exists(prog_path) and os.path.isfile(prog_path):
            return True
    return False


def my_chdir(folder):
    global CHDIR_LAST_DIRECTORY
    try:
        os.chdir(folder)
    except FileNotFoundError:
        if OPTIONS.only_echo_command:
            print(f"# Warning, folder {folder} does not yet exist")
        else:
            raise FileNotFoundError(f"# Cannot chdir to folder {folder} !")
    if OPTIONS.only_echo_command and folder != CHDIR_LAST_DIRECTORY:
        print(f"cd {folder}")
    CHDIR_LAST_DIRECTORY = folder


def run(cmd, chain_commands=False):
    if OPTIONS.only_echo_command:
        print(cmd)
        return

    if chain_commands and os.name == "nt":
        raise RuntimeError("chain_commands is not supported under windows")

    if chain_commands:
        cmd = _chain_and_echo_commands(cmd)
        subprocess.check_call(cmd, shell=True)
    else:
        if os.name == "nt":
            print("###### Run command ######")
            print(cmd)
            print()
            subprocess.check_call(cmd, shell=True)
        else:
            cmd = _cmd_to_echo_and_cmd(cmd)
            subprocess.check_call(cmd, shell=True)


def chdir(folder):
    global CHDIR_LAST_DIRECTORY
    try:
        os.chdir(folder)
    except FileNotFoundError:
        if OPTIONS.only_echo_command:
            print(f"# Warning, folder {folder} does not yet exist")
        else:
            raise FileNotFoundError(f"# Cannot chdir to folder {folder} !")
    if OPTIONS.only_echo_command and folder != CHDIR_LAST_DIRECTORY:
        print(f"cd {folder}")
    CHDIR_LAST_DIRECTORY = folder


def run(cmd, chain_commands=False):
    if OPTIONS.only_echo_command:
        print(cmd)
        return

    if chain_commands and os.name == "nt":
        raise RuntimeError("chain_commands is not supported under windows")

    if chain_commands:
        cmd = _chain_and_echo_commands(cmd)
        subprocess.check_call(cmd, shell=True)
    else:
        if os.name == "nt":
            print("###### Run command ######")
            print(cmd)
            print()
            subprocess.check_call(cmd, shell=True)
        else:
            cmd = _cmd_to_echo_and_cmd(cmd)
            subprocess.check_call(cmd, shell=True)


def _cmd_to_echo_and_cmd_lines(cmd: str) -> [str]:
    lines_with_echo = ["echo '###### Run command ######'", f"echo '{cmd}'", "echo ''", cmd]
    return lines_with_echo


def _cmd_to_echo_and_cmd(cmd: str) -> str:
    end_line = " &&         \\\n"
    return end_line.join(_cmd_to_echo_and_cmd_lines(cmd))


def _chain_and_echo_commands(commands: str):
    """
    Take a series of shell command on a multiline string (one command per line)
    and returns a shell command that will execute each of them in sequence,
    while echoing them, and ignoring commented lines (with a #)
    """
    lines = commands.split("\n")
    # strip lines
    lines = map(lambda s: s.strip(), lines)
    # suppress empty lines
    lines = filter(lambda s: not len(s) == 0, lines)

    # add "echo commands" and process comments:
    # comments starting with # are discarded and comments starting with ## are displayed loudly
    lines_with_echo = []
    for line in lines:
        if line.startswith("##"):
            echo_line = f"echo '******************** {line[2:].strip()} ***************'"
            lines_with_echo.append(echo_line)
        elif not line.startswith("#"):
            lines_with_echo = lines_with_echo + _cmd_to_echo_and_cmd_lines(line)

    # End of line joiner
    end_line = " &&         \\\n"

    r = end_line.join(lines_with_echo)
    r = r.replace("&& &&", "&& ")
    return r


def decorate_loudly_echo_function_name(fn):
    def wrapper_func(*args, **kwargs):
        msg = f"""
# ==================================================================
#                {fn.__name__}
# =================================================================="""
        if os.name == "nt":
            print(msg)
        else:
            for line in msg.split("\n"):
                subprocess.check_call(f"echo '{line}'", shell=True)

        return fn(*args, **kwargs)

    wrapper_func.__doc__ = fn.__doc__
    wrapper_func.__name__ = fn.__name__
    return wrapper_func


def do_clone_repo(git_repo, folder, branch="", tag="", init_submodules=True, pull=True):
    if not os.path.isdir(f"{EXTERNAL_DIR}/{folder}"):
        my_chdir(EXTERNAL_DIR)
        run(f"git clone {git_repo} {folder}")
    my_chdir(f"{EXTERNAL_DIR}/{folder}")
    if len(branch) > 0:
        run(f"git checkout {branch}")
    elif len(tag) > 0:
        run(f"git checkout {tag}")
    if pull:
        run(f"git pull")
    if init_submodules:
        run("git submodule update --init --recursive")
