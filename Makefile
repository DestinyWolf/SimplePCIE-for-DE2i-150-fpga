# Makefile para Driver e Aplicacao Terasic PCIe

# Configuracoes do Kernel
obj-m := altera_pcie.o
KERNELDIR?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Configuracoes da Aplicacao
APP_NAME = test_pcie
APP_SRC = main.cpp
SHOW_IP_NAME = show_ip
SHOW_IP_SRC = show_ip.cpp
CXX = g++
CXXFLAGS = -O2 -Wall
LIBS = -ldl
AUX_LIB =  SimplePCIE.cpp

.PHONY: all module app clean load unload

all: module app

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

app:
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(APP_SRC) $(LIBS)

show_ip:
	$(CXX) $(CXXFLAGS) -o $(SHOW_IP_NAME) $(SHOW_IP_SRC) $(AUX_LIB) $(LIBS)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f $(APP_NAME)
	rm -f $(SHOW_IP_NAME)

# Atalhos para facilitar o desenvolvimento
load:
	sudo insmod altera_pcie.ko
	sudo chmod 666 /dev/altera_pcie
	@echo "Driver carregado e permissoes ajustadas."

unload:
	sudo rmmod altera_pcie
	@echo "Driver removido."
