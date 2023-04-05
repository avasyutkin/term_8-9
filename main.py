from sympy.ntheory import totient


def doubling_P(P, a, p):
    λ = ['', '']
    λ[0] = (3 * P[0] ** 2 + a) % p
    λ[1] = 2 * P[1] % p
    if λ[1] == 0:
        return 0
    if not (λ[0]/λ[1]).is_integer():
        λ[0] = λ[0] % p
        λ[1] = (λ[1] ** (p - 2)) % p
        λ = int(λ[0] * λ[1] % p)
    else:
        λ = int(λ[0]/λ[1])

    x = (λ ** 2 - 2 * P[0]) % p
    y = (λ * (P[0] - x) - P[1]) % p

    return x, y


def addition_P(P1, P2, p):
    λ = ['', '']
    λ[0] = (P2[1] - P1[1]) % p
    λ[1] = (P2[0] - P1[0]) % p
    if λ[1] == 0:
        return 0
    if not (λ[0]/λ[1]).is_integer():
        λ[0] = λ[0] % p
        λ[1] = (λ[1] ** (p-2)) % p
        λ = int(λ[0] * λ[1] % p)
    else:
        λ = int(λ[0]/λ[1])

    x = (λ ** 2 - P1[0] - P2[0]) % p
    y = (λ * (P1[0] - x) - P1[1]) % p

    return x, y


def quadratic_residues(p):
    quadrate = {}

    for i in range(p):
        quadrate[i] = (i ** 2 % p)

    return set(quadrate.values())


def all_points(a, b, p, quadrate):
    y_quadrate = []

    for i in range(p):
        y_quadrate.append((i ** 3 + a * i + b) % p)

    points = []
    for i in range(p):
        for j in range(len(quadrate)):
            if y_quadrate[i] == quadrate[j]:
                points.append([i, j])
                #print('x =', i, '=> ', 'y^2 =', (i ** 3 + a * i + b), 'mod', p, '=', (i ** 3 + a * i + b) % p, '  y =', j)


    print('Точки кривой:', points)
    print()

    return points


def Legendre_symbol(a, b, p, quadrate):
    symbol = 0
    for j in range(len(quadrate)):

        if (j ** 3 + a * j + b) % p == 0:
            print('x =', j, '=> ', (j ** 3 + a * j + b), '/', p, '=', (j ** 3 + a * j + b) % p, '/', p, '=', 0)
            continue
        elif (j ** 3 + a * j + b) % p in quadrate.values():
            symbol += 1
            print('x =', j, '=> ', (j ** 3 + a * j + b), '/', p, '=', (j ** 3 + a * j + b) % p, '/', p, '=', 1)
        else:
            symbol -= 1
            print('x =', j, '=> ', (j ** 3 + a * j + b), '/', p, '=', (j ** 3 + a * j + b) % p, '/', p, '=', -1)

    print('Символ Лежандра:', symbol)

    symbol = p + 1 + symbol
    print('Порядок кривой:', symbol)
    print()

    return symbol


def invariant(a, b, p):
    invariant = (1728 * 4 * a ** 3) * (4 * (a ** 3) + 27 * (b ** 2)) ** totient(p - 1) % p
    print('Инвариант кривой равен:', invariant)
    print()

    return invariant



def point_orders(a, b, p):
    quadrate = quadratic_residues(p)
    points = all_points(a, b, p, quadrate)
    curve_order = Legendre_symbol(a, b, p, quadrate)
    invariant(a, b, p)

    for i in points:
        flag = False
        for l in range(2, curve_order + 1):
            T = i
            T_= T
            l_ = bin(l)[3:]

            for r in l_:
                if l == curve_order:
                    print(l, T, '=', 0)
                    print('Порядок точки', T, 'равен', l)
                    print()
                    flag = True
                    break

                T_ = doubling_P(T_, a, p)
                if T_ == 0:
                    print(l, T, '=', T_)
                    print('Порядок точки', T, 'равен', l)
                    print()
                    flag = True
                    break

                if r == '1':
                    T_ = addition_P(T, T_, p)
                    if T_ == 0:
                        print(l, T, '=', T_)
                        print('Порядок точки', T, 'равен', l)
                        print()
                        flag = True
                        break

            if flag:
                break
            print(l, T, '=', T_)

# point_orders(2, 6, 7)

# print(quadratic_residues(23))