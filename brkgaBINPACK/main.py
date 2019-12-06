boxes = [(1,2), (2,1)]

bins = [(4,4)]

population = [(0,1), (1,0)]

EMS = [(4,4)]
resetEMS = EMS.copy()
binsActualCapacity = [16]

def updateEmptySpaces(gene):
    print(gene)
    imax = 0
    size = 0
    for i in range(len(EMS)):
        if EMS[i][0] * EMS[i][1] > size:
            size = EMS[i][0] * EMS[i][1]
            imax = i
    
    space = EMS.pop(imax)
    EMS.append((space[0] - boxes[gene][0], space[1]))
    EMS.append((space[0], space[1] - boxes[gene][1]))

    binsActualCapacity[0] -= boxes[gene][0] * boxes[gene][1]


def fitness():

    NB = len(bins)

    for chromossome in population:
        for gene in chromossome:
            updateEmptySpaces(gene)
        
    
    leastLoad = float('inf')

    for load in binsActualCapacity:
        if load < leastLoad:
            leastLoad = load

    return NB - leastLoad / (bins[0][0] * bins[0][1]) #bin capacity

def genetic():
    fitness_value = fitness()

    print(fitness_value)




def main():
    genetic()

if __name__ == "__main__":
    main()