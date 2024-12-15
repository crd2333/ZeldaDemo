from PIL import Image
import array
import math
import os
import sys
import time

imgs = []

def generate_mdb(n):
    mdb = []
    i = 0
    while len(mdb) < n:
        if i & 0x55555555 == i:
            mdb.append(i)
        i += 1
    return mdb


def z_from_xy(x, y, mdb):
    return mdb[x] + 2*mdb[y]

def grid2origin(grid_left, grid_top, grid_right, grid_bottom, detail):
    grid_size = 2**detail
    return (grid_left/grid_size, grid_top/grid_size, grid_right/grid_size, grid_bottom/grid_size)

def origin2grid(grid_left, grid_top, grid_right, grid_bottom, detail):
    grid_size = 2**detail
    return [math.floor(grid_left*grid_size), math.floor(grid_top*grid_size),
            math.ceil(grid_right*grid_size+0.00001) - 1, math.ceil(grid_bottom*grid_size+0.00001) - 1]

def draw_grid(grid_left, grid_top, grid_x, grid_y, detail, image, mdb):
    name = '5' + str(detail)
    grid_z = z_from_xy(grid_x, grid_y, mdb)

    grid_rel_x = 256*(grid_x - grid_left)
    grid_rel_y = 256*(grid_y - grid_top)

    name += format(grid_z, '0>8X')
    file = None
    try:
        file = open('terrain/' + name + '.hght', 'rb')
    except:
        # print('\nFile not found: ' + name + '.hght')
        return

    h = array.array('H')
    h.fromfile(file, 65536)
    h = h.tobytes()  # Modified line
    temp = Image.frombytes('I;16', (256, 256), h)

    image.paste(temp, (grid_rel_x, grid_rel_y))

    file.close()

def draw_map(grid_left, grid_top, grid_right, grid_bottom, detail, image, mdb):
    width = grid_bottom - grid_top
    for y in range(grid_top, grid_bottom + 1):
        # frac = 1
        # if width != 0:
        #     frac = (y - grid_top) / width
        # fill = round(frac*40)
        # print('\r[', '█'*fill + ' '*(40-fill), '] {:>7.2%}'.format(frac), sep='', end='')
        # sys.stdout.flush()
        for x in range(grid_left, grid_right + 1):
            draw_grid(grid_left, grid_top, x, y, detail, image, mdb)

def create_image(name=str(int(time.time()))):
    # region of GreatPlateau （初始台地 in Chinese）
    left = 0.405
    top = 0.59
    right = 0.475
    bottom = 0.65
    min_LOD = 5
    max_LOD = 8
    for detail in range(min_LOD, max_LOD + 1):
        print(f"generating image for detail level {detail}...")
        grid_size = 2**detail
        grid_left, grid_top, grid_right, grid_bottom = origin2grid(left, top, right, bottom, detail)

        mdb = generate_mdb(grid_size)

        img_size = (256*(1 + grid_right - grid_left), 256*(1 + grid_bottom - grid_top))
        img = Image.new('I', img_size)
        imgs.append(img)

        draw_map(grid_left, grid_top, grid_right, grid_bottom, detail, img, mdb)
        img.save('hghtmap_{}.png'.format(detail), 'PNG')
    print('Completed!')

if __name__ == "__main__":
    create_image()
