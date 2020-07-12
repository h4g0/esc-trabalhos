

def compute_metrics(instructions,cycles,LLC_loads,LLC_load_misses,L1_dcache_loads,L1_dcache_load_misses,dTLB_load_misses,branch_misses,branches):
    
    print(f"instructions per cycle {instructions / cycles}")
    
    print(f"L1 cache miss ration {L1_dcache_loads /  L1_dcache_load_misses}")
    print(f"L1  cache miss rate PTI {L1_dcache_loads / (instructions / 1000)}")

    print(f"LLC cache miss ration {LLC_loads /  LLC_load_misses}")
    print(f"LLC  cache miss rate PTI {LLC_load_misses / (instructions / 1000)}")
    
    print(f"Data TLB miss rate PTI {dTLB_load_misses / (instructions / 1000)}")
    
    print(f"Branch mispredict ratio {branch_misses/branches}")
    print(f"Branch mispredict rate PTI {branch_misses / (instructions / 1000)}")

    metrics = list()

    metrics.append(instructions / cycles)
    
    metrics.append(L1_dcache_loads /  L1_dcache_load_misses)
    metrics.append(L1_dcache_loads / (instructions / 1000))

    metrics.append(LLC_loads /  LLC_load_misses)
    metrics.append(LLC_load_misses / (instructions / 1000))
    
    metrics.append(dTLB_load_misses / (instructions / 1000))
    
    metrics.append(branch_misses/branches)
    metrics.append(branch_misses / (instructions / 1000))

    return metrics


def print_metrics(metrics):
    for i in range(len(metrics[0])):
        row_elements = [str(round(m[i],3)) for m in metrics]
        row_elements = " & ".join(row_elements)
        print(f"{row_elements} \\\\")
            
def main():
             

    metrics = list()
    print("\n--------------------------------2048-----------------------------------\n")
    aux = compute_metrics(60861147730,145812500000,2144100000,437700000,1531700000,438200000,2421900000,4400000,8756400000)
    metrics.append(aux)
    print("")
    aux = compute_metrics(22086293233,16049600000,97500000,80900000,191000000,81200000,12800000,4200000,2309400000)
    metrics.append(aux)

    print("\n--------------------------------512-----------------------------------\n")
    aux = compute_metrics(980082628,953000000,8700000,1,8900000,1,9000000,200000,143900000)
    metrics.append(aux)
    print("")
    aux = compute_metrics(377841466,183900000,1700000,1,1800000,1,400000,200000,44100000)
    metrics.append(aux)


    print_metrics(metrics)

main()
