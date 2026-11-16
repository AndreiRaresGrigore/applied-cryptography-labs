import socket
import hashlib
#connection of Bob (client) to Alice (server)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as soc:
    soc.connect(("192.168.1.7", 12345))
    #receives the hash from Alice
    alice_hash = soc.recv(1024).decode()
    print("\nAlice hash:", alice_hash,"\n")
    #Bobs move sended to Alice
    inp = input("Bob -> rock, paper, scissors: ")
    soc.send(inp.encode())
    #receving Alices move and salt and generate the hash
    alice_move = soc.recv(1024).decode()
    ainp, sal = alice_move.split("|")
    print("\nAlice input:", ainp)
    print("\nAlice salt:", sal)
    computed_hash = hashlib.sha256((ainp + sal).encode()).hexdigest()
    #compare the first hash received with the computed one
    if computed_hash == alice_hash:
        print("\nGOOD - Alice didnt cheat")
        if ainp == inp:
            result = "Draw"
        elif (ainp == "rock" and inp == "scissors") or \
            (ainp == "paper" and inp == "rock") or \
            (ainp == "scissors" and inp == "paper"):
            result = "Alice won"
        else:
            result = "Bob won"
        print("\nActual hash", computed_hash)
        print("\nOld hash", alice_hash)
        print("\nResult:", result)
    else:
        print("\nBAD - Alice cheated")
        print("\nActual hash", computed_hash)
        print("\nOld hash", alice_hash)
