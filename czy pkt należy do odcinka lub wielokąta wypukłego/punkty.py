def pozycja_punktu(A, B, C):
    x1, y1 = A
    x2, y2 = B
    x3, y3 = C

    kolinearne = (x2 - x1) * (y3 - y1) == (y2 - y1) * (x3 - x1)
    
    na_odcinku = kolinearne and x3 >= min(x1, x2) and x3 <= max(x1, x2) and y3 >= min(y1, y2) and y3 <= max(y1, y2)

    if na_odcinku:
        return "Punkt C należy do odcinka AB."
    elif kolinearne:
        return "Punkt C leży na prostej AB, ale poza odcinkiem."
    else:
        return "Punkt C nie leży na prostej AB."

def wczytaj_punkt(nazwa):
    x = float(input(f"Podaj współrzędną x dla punktu {nazwa}: "))
    y = float(input(f"Podaj współrzędną y dla punktu {nazwa}: "))
    return (x, y)

A = wczytaj_punkt("A")
B = wczytaj_punkt("B")
C = wczytaj_punkt("C")

print(pozycja_punktu(A, B, C))
