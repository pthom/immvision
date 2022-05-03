import logging
import inspect


def log(level, message):
    "Automatically log the current function details."
    import inspect, logging
    # Get the previous frame in the stack, otherwise it would
    # be this function!!!
    func = inspect.currentframe().f_back.f_code
    # Dump the message + the name of this function to the log.
    logging.log(level, "%s: %s at %s:%i" % (
        message,
        func.co_name,
        func.co_filename,
        func.co_firstlineno
    ))


def verbose_function(dump_args: bool = True, dump_return: bool = False, dump_args_at_exit: bool = False):
    """
    Decorator to print function call details.
    This can include:
     * input parameters names and effective values
     * output parameters (if they were modified by the function)
     * return value
    :param dump_args: output function args
    :param dump_return: output function return values
    :param dump_args_at_exit: output function args at exit, for functions that modify their input args
    """
    def inner(func):
        def wrapper(*args, **kwargs):
            def indent(s: str, indent_size: int):
                return "\n".join(map(lambda s: " " * indent_size + s, s.split("\n")))

            def do_dump_arg_multiline(arg):
                arg_name = str(arg[0])
                arg_value_str = str(arg[1])
                if "\n" in arg_value_str:
                    return f"{arg_name} = \n{indent(arg_value_str, 4)}"
                else:
                    return f"{arg_name} = {arg_value_str}"

            def do_dump_args(joining_str: str):
                try:
                    # For standard functions, inspect the signature
                    signature = inspect.signature(func)
                    func_args = signature.bind(*args, **kwargs).arguments
                    func_args_strs = map(do_dump_arg_multiline, func_args.items())
                    func_args_str = joining_str.join(func_args_strs)
                    arg_str =  f"{func_args_str}"
                except ValueError:
                    # For native functions, the signature cannot be inspected
                    annotated_args = map(lambda arg_and_idx: (f"arg_{arg_and_idx[0]}", str(arg_and_idx[1])), enumerate(args))
                    args_strs = list(map(do_dump_arg_multiline, annotated_args ))
                    kwargs_strs = list(map(do_dump_arg_multiline, kwargs.items()))
                    func_args_str = joining_str.join(args_strs + kwargs_strs)
                    arg_str =  f"{func_args_str}"
                return arg_str

            def do_dump_fn_name():
                try:
                    # For standard functions, inspect the signature
                    fn_str = f"{func.__module__}.{func.__qualname__}"
                    return fn_str
                except AttributeError:
                    # For native functions, the signature cannot be inspected
                    fn_str = f"{func.__module__}.{func.__name__}"
                    return fn_str

            join_str = "\n" if dump_args_at_exit else ", "

            initial_args_str = f"{do_dump_args(join_str)}"
            if not dump_args_at_exit:
                initial_args_str = f"({initial_args_str})"
            if not dump_args:
                initial_args_str = ""

            function_output = func(*args, **kwargs)

            function_output_str = str(function_output)
            final_args_str = do_dump_args(join_str)
            function_name = do_dump_fn_name()

            if not dump_args_at_exit:
                if dump_return:
                    msg = f"{function_name}{initial_args_str} -> {function_output_str}"
                else:
                    msg = f"{function_name}{initial_args_str}"
            else:
                msg = f"{do_dump_fn_name()}\n"
                msg += f"    args in : \n{indent(initial_args_str, 8)}\n"
                msg += f"    args out: \n{indent(final_args_str, 8)}\n"
                if dump_return:
                    msg += f"    return  : \n{indent(function_output_str, 8)}"

            logging.debug(msg)
            return function_output

        return wrapper
    return inner


from dataclasses import dataclass

@dataclass
class TwoNumbers:
    a: int = 0
    b: int = 0
    sum: int = 0


@verbose_function(dump_return=False, dump_args_at_exit=False)
def add_simple(two_number: TwoNumbers):
    r = two_number.a + two_number.b
    return r


@verbose_function(dump_return=True, dump_args_at_exit=False)
def add_dump_return(two_number: TwoNumbers):
    r = two_number.a + two_number.b
    return r


@verbose_function(dump_return=False, dump_args_at_exit=True)
def add_dump_exit(two_number: TwoNumbers):
    two_number.sum = two_number.a + two_number.b


@verbose_function(dump_return=True, dump_args_at_exit=True)
def add_dump_return_exit(two_number: TwoNumbers):
    two_number.sum = two_number.a + two_number.b


def test_dump_args_python_functions():
    add_simple(TwoNumbers(1, 2))
    add_dump_return(TwoNumbers(1, 2))
    add_dump_exit(TwoNumbers(1, 2))
    add_dump_return_exit(TwoNumbers(1, 2))


def test_dump_args_native_functions():
    import cv2
    import numpy as np
    m = np.zeros((2, 2, 3), np.uint8)
    m[:,:,:] = (1, 2, 3)
    cv2.cvtColor = verbose_function(dump_return=True, dump_args_at_exit=True)(cv2.cvtColor)
    m2 = np.zeros((2, 2, 3), np.uint8)
    cv2.cvtColor(m, cv2.COLOR_BGR2GRAY, dst=m2)
