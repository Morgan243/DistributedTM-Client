#need to add support for cross compile
COMP=g++

CLIENT_PATH=../NetComm.git/

#we'll want to try and use C11 threads if the cross compiler can do it
FLAGS=-lpthread --std=c++11
OUT=test


all : main.o TM_Client.o TM_Share.o $(CLIENT_PATH)NC_Client.o $(CLIENT_PATH)/NetComm.o
	$(COMP) main.o TM_Client.o TM_Share.o $(CLIENT_PATH)NC_Client.o $(CLIENT_PATH)/NetComm.o -o $(OUT)

main.o : main.cpp 
	$(COMP) -c main.cpp $(FLAGS)

TM_Client.o : TM_Client.cpp TM_Client.h 
	$(COMP) -c TM_Client.cpp TM_Client.h $(FLAGS)


TM_Share.o : TM_Share.cpp TM_Share.h
	$(COMP) -c TM_Share.cpp TM_Share.h $(FLAGS)
	
clean :
	rm $(OUT) *.o *.gch
