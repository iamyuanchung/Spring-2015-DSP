# coding=Big5


def main():
    ZY = {}
    with open('Big5-ZhuYin.map', 'rb') as f:
        for line in f:
            data = line.split(' ')
            chi = data[0]   # the Chinese character
            zy_list = data[1].split('/')    # the corresponding list of Zhu-Yin notations
            zy_list[-1] = zy_list[-1][:-1]  # remove '\n' in the last Zhu-Yin notation
            for each_zy in zy_list:
                if each_zy[:2] in ZY:
                    ZY[each_zy[:2]].append(chi)
                else:
                    ZY[each_zy[:2]] = [chi]
    for each_key in ZY:
        ZY[each_key] = list(set(ZY[each_key]))
    for key in ZY:
        print key,
        for value in ZY[key]:
            print value,
        print ''
        for value in ZY[key]:
            print value, value


if __name__ == '__main__':
    main()
