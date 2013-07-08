#~ parameter["spacing"] = 1/6.
parameter["shift"] = "shift1.txt"
#~ gl = lambda x:[l for l in drange(0, x + parameter["spacing"] + 0.000001, parameter["spacing"])]
gl = lambda x: range(0, shift.max_steps(x, x)+1)
parameter["dirs"]  = [[l, gl(l)] for l in range(60, 61, 2)]
parameter["args"]   = [["L", "H"], "g"]
parameter["bash"]  = "-shift shift.txt -mult 10 "
parameter["sq"]    = "-r 4h --mpp 500m"
parameter["files"] = ["../../build/examples/sim"]
parameter["cmake"] = "-DUSE_S:STRING=2 -DUSE_GRID:STRING=3"
