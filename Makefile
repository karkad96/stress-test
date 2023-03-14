MAKE_SERVER=make -C ./server && ln -s ../server/server ./ODP/server
MAKE_CHILD=make -C ./child && ln -s ../child/child ./ODP/child
MAKE_CLIENT=make -C ./client && ln -s ../client/client ./ODP/client
MAKE_HOOLIGAN=make -C ./hooligan && ln -s ../hooligan/hooligan ./ODP/hooligan
MAKE_ARBITER=make -C ./arbiter && ln -s ../arbiter/arbiter ./ODP/arbiter

all:
	$(MAKE_SERVER)
	$(MAKE_CHILD)
	$(MAKE_CLIENT)
	$(MAKE_HOOLIGAN)
	$(MAKE_ARBITER)

clean:
	cd ./ODP && rm child arbiter server hooligan client
	make -C ./server clean
	make -C ./arbiter clean
	make -C ./child clean
	make -C ./hooligan clean
	make -C ./client clean
