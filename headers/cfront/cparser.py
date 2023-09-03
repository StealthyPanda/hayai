import sys, os, json
import colorama as clr
from typing import List
import time
import pycparser

#----------------------------------------------------------------------------------------------------------------
#setup stuff
levels = [clr.Fore.WHITE, clr.Fore.GREEN, clr.Fore.RED, clr.Fore.YELLOW]
brightness = [clr.Style.NORMAL, clr.Style.BRIGHT, clr.Style.DIM]

def log(text : str, level : int = 0, br : int = 0) -> None:
    print(
        f"[{time.strftime('%H:%M:%S %d/%m/%Y')}] " +
        (levels[level]) + brightness[br] + text + clr.Style.RESET_ALL
    )


superverbose = '-vv' in sys.argv
verbose = ('-v' in sys.argv) or superverbose
verbocity = ''
if verbose: verbocity = '-v'
if superverbose: verbocity = '-vv'

cwd = os.getcwd()


filepath = ""
outputpath = ""

if '-f' not in sys.argv:
    log("No input file given!", level = -2)
    sys.exit(69)
else:
    try : filepath = sys.argv[sys.argv.index('-f') + 1]
    except IndexError:
        log("No input file given!", level = -2)
        sys.exit(69)

if '-o' in sys.argv:
    try : outputpath = sys.argv[sys.argv.index('-o') + 1]
    except IndexError:
        log("No output file given!", level = -2)
        sys.exit(69)

if outputpath == "":
    outputpath = os.path.join(cwd, f"{os.path.basename(filepath)}.il")

if not os.path.isabs(filepath):
    filepath = os.path.join(cwd, os.path.basename(filepath))
if not os.path.isabs(outputpath):
    outputpath = os.path.join(cwd, os.path.basename(outputpath))

filedir = os.path.dirname(filepath)

if superverbose:
    log(f"Input file  : {filepath}")
    log(f"Output file : {outputpath}")

filename = os.path.basename(filepath)
filedir = os.path.dirname(filepath)

# rawcode = ""
# with open(filename, 'r') as file:
#     rawcode = file.read()
#----------------------------------------------------------------------------------------------------------------


#----------------------------------------------------------------------------------------------------------------
#precprocessing (removing comments and stuff from rawcode), and tokenizing
dotppc = os.path.join(filedir, filename.split(".")[0]) + '.ppc'
flibs = 'C:\\Python311\\Lib\\site-packages\\pycparser\\fake_libc_include'
comm = f'gcc -E -nostdinc {filepath} -o {dotppc} -I {flibs}'

if superverbose:
    log(f"Flibs path : {flibs}")
    log(f"PPCommand  : {comm}")

c = os.system(comm)
if c != 0 : sys.exit(69)
if verbose: log(f"\n👍Preprocessed C file {filepath} -> {dotppc}", br = -1)
# code = []
# with open(dotppc, 'r') as file:
#     for each in file:
#         if each[0] == '#' : continue
#         code.append(each)

# code = '\n'.join(code)



#----------------------------------------------------------------------------------------------------------------






#----------------------------------------------------------------------------------------------------------------
#functions and stuff

ctypedict = {
    'int' : 'i32',
    'double' : 'f64',
    'float' : 'f32',
    'char' : 'byte',
    # 'char' : 'byte',
    # 'char' : 'byte',
    # 'char' : 'byte',
}



def proctype(t : pycparser.c_parser.c_ast.TypeDecl |
                 pycparser.c_parser.c_ast.PtrDecl  |
                 pycparser.c_parser.c_ast.IdentifierType) -> str:
    if type(t) == pycparser.c_parser.c_ast.IdentifierType:
        if t.names[0] in ctypedict.keys():
            return ctypedict[t.names[0]]
        return t.names[0]
    elif type(t) == pycparser.c_parser.c_ast.TypeDecl:
        return proctype(t.type)
    elif type(t) == pycparser.c_parser.c_ast.PtrDecl:
        inner = proctype(t.type)
        if 'ptr' in inner:
            inner = inner.split('[')
            inner = ' '.join(inner)
            inner = inner.split(']')
            inner = ' '.join(inner)
            inner = inner.split(' ')
            inner = list(filter(lambda x: x, inner))
            inner[inner.index('ptr') + 1] = f'[{int(inner[inner.index("ptr") + 1]) + 1}]'
            inner = ' '.join(inner)
            return inner
        return f'unsafe ptr[1] {inner}'

def procfparams(params : pycparser.c_parser.c_ast.ParamList) -> str:
    if params == None: return ''
    paramslist = ""
    for each in params.params:
        paramslist += f'clone {proctype(each.type)} {each.name}, '
    return paramslist[:-2]

def procfunc(func : pycparser.c_parser.c_ast.FuncDef) -> str:
    # print(func)
    return (
        f"\n@sig fun {func.decl.name}({procfparams(func.decl.type.args)}) :" +
        f" {proctype(func.decl.type.type)} " +
         '{}\n'
    )

def procvar(var : pycparser.c_parser.c_ast.Decl) -> str:
    return (
        f"\n{'const' if 'const' in var.quals else 'let'} {var.type.type.names[0]} {var.name} ;\n"
    )

def procstruct(struct : pycparser.c_parser.c_ast.Struct) -> str:
    if type(struct) == pycparser.c_parser.c_ast.Struct:
        structstring = f"\nstruct {struct.name} " + "{\n"
        for each in struct.decls:
            structstring += f"\t{proctype(each.type)} {each.name};\n"
        structstring += '}\n'
        return structstring
    else:
        name = struct.name
        struct = struct.type.type
        structstring = f"\nstruct {name} " + "{\n"
        if struct.decls:
            for each in struct.decls:
                structstring += f"\t{proctype(each.type)} {each.name};\n"
        structstring += '}\n'
        return structstring

def procdeftype(t : pycparser.c_parser.c_ast.Typedef) -> str:
    return (
        f"\n@def {t.type.type.names[0]} {t.name} \n"
    )


#----------------------------------------------------------------------------------------------------------------









#----------------------------------------------------------------------------------------------------------------
#main stuff

ast = pycparser.parse_file(dotppc)
if superverbose:
    pass

# dotil = os.path.join(filedir, filename.split(".")[0]) + '.il'
dotil = outputpath
dic = []

# print(len(ast.children()))
try:
    for i, (e, node) in enumerate(ast.children()):
        # print(node)
        if type(node) == pycparser.c_parser.c_ast.FuncDef:
            # print('found func')
            dic.append(procfunc(node))
        if type(node) == pycparser.c_parser.c_ast.Decl:
            # print(node)
            if type(node.type) == pycparser.c_parser.c_ast.Struct:
                # print('found struct')
                dic.append(procstruct(node.type))
            else:
                # print('found var')
                dic.append(procvar(node))
        if ((type(node) == pycparser.c_parser.c_ast.Typedef)
            and (type(node.type) == pycparser.c_parser.c_ast.TypeDecl)
            and (type(node.type.type) == pycparser.c_parser.c_ast.Struct)):
            # print('found struct')
            # print(node)
            dic.append(procstruct(node))
        if ((type(node) == pycparser.c_parser.c_ast.Typedef)
            and (type(node.type) == pycparser.c_parser.c_ast.TypeDecl)
            and (type(node.type.type) == pycparser.c_parser.c_ast.IdentifierType)):
            # print('found struct')
            # print(node)
            dic.append(procdeftype(node))
except:
    print(node)
    exit(69)


with open(dotil, 'w') as file:
    file.write(''.join(dic))

if verbose:
    log(f"\n👍Processed C file {filepath} -> {outputpath}", level=1, br=1)

#----------------------------------------------------------------------------------------------------------------

