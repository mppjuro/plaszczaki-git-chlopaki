
def czy_punkty_są_współliniowe(A, B, C):
    return (B[0] - A[0]) * (C[1] - A[1]) == (B[1] - A[1]) * (C[0] - A[0])

def czy_punkt_należy_do_krawędzi(P, A, B):
    if czy_punkty_są_współliniowe(P, A, B):
        if (A[0] <= P[0] and P[0] <= B[0]) or (A[0] >= P[0] and P[0] >= B[0]):
            if (A[1] <= P[1] and P[1] <= B[1]) or (A[1] >= P[1] and P[1] >= B[1]):
                return True
    return False

def czy_półprosta_przecina_krawędź(P, A, B):
    if (A[0] <= P[0] and P[0] < B[0]) or (A[0] >= P[0] and P[0] > B[0]):
        if P[1] > (A[1] - B[1]) / (A[0] - B[0]) * (P[0] - A[0]) + A[1]:
            return True
    return False

def punkt_w_wielokącie(P, wierzchołki):
    n = len(wierzchołki)
    for i in range(n):
        A = wierzchołki[i]
        B = wierzchołki[(i + 1) % n]
        if czy_punkt_należy_do_krawędzi(P, A, B):
            return "Punkt P należy do wielokąta"
    
    max_x = min_x = wierzchołki[0][0]
    for i in range(1, n):
        if wierzchołki[i][0] > max_x:
            max_x = wierzchołki[i][0]
        elif wierzchołki[i][0] < min_x:
            min_x = wierzchołki[i][0]

    if P[0] == max_x or P[0] == min_x:
        return "Punkt P nie należy do wielokąta"

    przecięcia = 0
    for i in range(n):
        A = wierzchołki[i]
        B = wierzchołki[(i + 1) % n]
        if czy_półprosta_przecina_krawędź(P, A, B):
            przecięcia += 1

    if przecięcia % 2 == 0:
        return "Punkt P nie należy do wielokąta"
    else:
        return "Punkt P należy do wielokąta"

# Przykładowe dane
wierzchołki_wielokąta = [(1, 1), (5, 1), (5, 5), (1, 5)]
punkt_P = (3, 35)

print(punkt_w_wielokącie(punkt_P, wierzchołki_wielokąta))
