def czy_po_tej_samej_stronie(p1, p2, a, b):
    w1 = (b[0] - a[0]) * (p1[1] - a[1]) - (b[1] - a[1]) * (p1[0] - a[0])
    w2 = (b[0] - a[0]) * (p2[1] - a[1]) - (b[1] - a[1]) * (p2[0] - a[0])
    return w1 * w2 >= 0

def czy_na_krawedzi(punkt, a, b):
    na_linii = (b[0] - a[0]) * (punkt[1] - a[1]) - (b[1] - a[1]) * (punkt[0] - a[0]) == 0
    w_zakresie_x = min(a[0], b[0]) <= punkt[0] <= max(a[0], b[0])
    w_zakresie_y = min(a[1], b[1]) <= punkt[1] <= max(a[1], b[1])
    return na_linii and w_zakresie_x and w_zakresie_y

def czy_wewnatrz(punkt, wierzcholki):
    for i in range(len(wierzcholki)):
        nastepny = (i + 1) % len(wierzcholki)
        if czy_na_krawedzi(punkt, wierzcholki[i], wierzcholki[nastepny]):
            return True
        if not czy_po_tej_samej_stronie(punkt, wierzcholki[nastepny], wierzcholki[i], wierzcholki[nastepny]):
            return False
    return True

def wczytaj_punkt(nazwa):
    x = float(input(f"Podaj współrzędną x dla punktu {nazwa}: "))
    y = float(input(f"Podaj współrzędną y dla punktu {nazwa}: "))
    return (x, y)

n = int(input("Podaj liczbę wierzchołków wielokąta: "))
wierzcholki = [wczytaj_punkt(f"{i+1}") for i in range(n)]

punkt = wczytaj_punkt("do sprawdzenia")

if czy_wewnatrz(punkt, wierzcholki):
    print("Punkt znajduje się wewnątrz wielokąta lub na jego krawędzi.")
else:
    print("Punkt znajduje się na zewnątrz wielokąta.")
