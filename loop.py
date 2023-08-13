import time
import colorama

def showloop(seq : list, delay = 0.1):
    i = 0
    while True:
        print(f"\r{colorama.Fore.GREEN + seq[i % len(seq)] + colorama.Fore.GREEN}  ", end='')
        time.sleep(delay)
        i += 1

slides = ["(", "⁀", " )", "‿"]
emotes = "⣾⣽⣻⢿⡿⣟⣯⣷"
showloop(emotes, 0.3)