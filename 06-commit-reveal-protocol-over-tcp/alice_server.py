import socket
import hashlib
import secrets
#Alice move, generation of salt and hash
inp = input("Alice -> rock, paper, scissors: ")
sal = secrets.token_hex(16)
mes = inp + sal
C = hashlib.sha256(mes.encode()).hexdigest()
print("\nAlice hash:", C)
#create the server socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as soc:
    soc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    soc.bind(("0.0.0.0", 12345))
    soc.listen()
    print("\nwaiting for Bob...")
    #Bobs connection to Alice
    conn, addr = soc.accept()
    with conn:
        print("\nBob connected:", addr)
        #sends to Bob the hash
        conn.send(C.encode())
        #receives Bobs input
        bobinp = conn.recv(1024).decode()
        print("\nBob inserted:", bobinp)
        inp = input("Insert your move again or try to cheat -> rock, paper, scissors: ")
        #sends to bob the final input and salt
        reveal_msg = f"{inp}|{sal}"
        conn.send(reveal_msg.encode())
print("\nAlice sended:", reveal_msg)
