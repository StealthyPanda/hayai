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
    outputpath = os.path.join(cwd, f"output_cfront_{os.path.basename(filepath)}.il")

if not os.path.isabs(filepath):
    filepath = os.path.join(cwd, os.path.basename(filepath))
if not os.path.isabs(outputpath):
    outputpath = os.path.join(cwd, os.path.basename(outputpath))

filedir = os.path.dirname(filepath)

if superverbose:
    print("Input file  :", filepath)
    print("Output file :", outputpath)

filename = os.path.basename(filepath)
filedir = os.path.dirname(filepath)

# rawcode = ""
# with open(filename, 'r') as file:
#     rawcode = file.read()
#----------------------------------------------------------------------------------------------------------------


#----------------------------------------------------------------------------------------------------------------
#precprocessing (removing comments and stuff from rawcode), and tokenizing
dotppc = os.path.join(filedir, filename.split(".")[0]) + '.ppc'

c = os.system(f'gcc -E {filepath} -o {dotppc}')
if c != 0 : sys.exit(69)

# code = []
# with open(dotppc, 'r') as file:
#     for each in file:
#         if each[0] == '#' : continue
#         code.append(each)

# code = '\n'.join(code)



#----------------------------------------------------------------------------------------------------------------






#----------------------------------------------------------------------------------------------------------------
#functions and stuff

def procfparams(params : pycparser.c_parser.c_ast.ParamList) -> str:
    paramslist = ""
    for each in params.params:
        paramslist += f'{each.type.type.names[0]} {each.name},'

def procfunc(func : pycparser.c_parser.c_ast.FuncDef) -> str:
    print(func)
    return (
        f"\nfun {func.decl.name}() : {func.decl.type.type.type.names[0]} " +
         '{}\n'
    )

def procvar(var : pycparser.c_parser.c_ast.Decl) -> str:
    return (
        f"\n{'const' if 'const' in var.quals else 'let'} {var.type.type.names[0]} {var.name} ;\n"
    )


#----------------------------------------------------------------------------------------------------------------









#----------------------------------------------------------------------------------------------------------------
#main stuff

ast = pycparser.parse_file(dotppc)
if superverbose:
    # ast.show()
    pass

dotil = os.path.join(filedir, filename.split(".")[0]) + '.il'
dic = ""

for i, (e, node) in enumerate(ast.children()):
    if type(node) == pycparser.c_parser.c_ast.FuncDef:
        dic += procfunc(node)
    if type(node) == pycparser.c_parser.c_ast.Decl:
        dic += procvar(node)


with open(dotil, 'w') as file:
    file.write(dic)

#----------------------------------------------------------------------------------------------------------------

