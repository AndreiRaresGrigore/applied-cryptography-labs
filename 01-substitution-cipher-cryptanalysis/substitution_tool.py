"""
Interactive helper used to break a monoalphabetic substitution cipher
with frequency analysis (functions as written for the assignment, Italian names kept).

- aggiungi_a_lista_sostituzioni("X=Y")  -> map ciphertext symbol X to plaintext Y
- applica_lista_sostituzioni(text, map, highlight) -> apply the mapping (substituted
  characters can be highlighted in green to track progress)
- frequenza(text) -> letter frequencies, to compare with English frequencies

Running the file applies the final key recovered in the report to ciphertext.txt.
"""
import argparse
import os

diz_sost = {}


def aggiungi_a_lista_sostituzioni(input):
    x, y = input.split("=", 1)
    diz_sost[x] = y
    print("Associata lettera-> " + y + " ------a lettera -> " + x)


def applica_lista_sostituzioni(testo, diz, evidenzia):
    if not diz:
        return testo

    lista = list(diz.keys())
    ris = ""
    i = 0

    while i < len(testo):
        trovato = False
        j = 0

        while j < len(lista):
            primelem = lista[j]
            if testo[i : i + len(primelem)] == primelem:
                x = diz[primelem]
                if evidenzia:
                    ris += "\033[32m" + x + "\033[0m"
                else:
                    ris += x
                i += len(primelem)
                trovato = True
                break
            j += 1

        if not trovato:
            ris += testo[i]
            i += 1

    return ris


def frequenza(testo):
    freq = {}
    tot = 0
    for i in testo:
        if i == "\n" or i == "\\" or i == "n":
            continue
        freq[i] = freq.get(i, 0) + 1
        tot += 1

    print("FREQUENZE:")
    for i in sorted(freq, key = freq.get, reverse = True):
        percentuale = freq[i] / tot * 100
        print("''", i, "''", "->", round(percentuale, 3), "%")


# Final key recovered step by step in the report (ciphertext -> plaintext).
# Order matters: "TT" (newline) must be tried before "T" (space).
FINAL_KEY = {
    "TT": "\n", "T": " ", "E": "E", "O": "T", "U": "H", "I": "A", "H": "O", "J": "R", "W": "I",
    "S": "S", "A": "N", "B": "D", "C": "L", "F": "W", "L": "G", " ": "F", "Y": "M", "Q": "C",
    "R": "U", "G": "Y", "D": "V", "N": "B", "X": "K", "P": "P", "K": "J", "V": "X", "M": "Z",
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Decrypt ciphertext.txt with the recovered key.")
    parser.add_argument("--freq", action="store_true", help="print the ciphertext letter frequencies first")
    parser.add_argument("--highlight", action="store_true", help="show substituted characters in green")
    args = parser.parse_args()

    here = os.path.dirname(os.path.abspath(__file__))
    # line breaks in the file are only for readability: "TT" encodes the real newlines
    with open(os.path.join(here, "ciphertext.txt"), encoding="utf-8") as f:
        cifrato = f.read().replace("\n", "")

    if args.freq:
        frequenza(cifrato)
        print()
    print(applica_lista_sostituzioni(cifrato, FINAL_KEY, args.highlight), end="")
