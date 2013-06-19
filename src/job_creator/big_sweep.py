parameter["spaceing"] = 1/4.
gl = lambda x:[j for j in drange(0, x+parameter["spaceing"]+0.000001, parameter["spaceing"])]

parameter["dirs"]  = [[i, gl(i)] for i in range(4, 5, 20)]
parameter["arg"]   = [["L", "H"], "g"]
parameter["bash"]  = "-shift shift.txt -mult 1 -spaceing " + str(parameter["spaceing"])
parameter["sq"]    = "-r 8h --mpp 500m"
parameter["files"] = ["../../build/examples/sim"]
parameter["cmake"] = "-DUSE_S:STRING=3 -DUSE_GRID:STRING=4"
