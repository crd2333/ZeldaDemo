import re

"""
从 3dsmax 把模型的 appearence 保存到 vertex color 中，导出为 ase 文件
从 ase 文件中解析出顶点、颜色和面数据
"""

def parse_ase(file_path):
    vertices = []
    colors = []
    faces = []
    in_vertex_list = False
    in_vertcol_list = False
    in_face_list = False

    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if line.startswith('*MESH_VERTEX_LIST'):
                continue
            elif line.startswith('*MESH_VERTCOL_LIST'):
                continue
            elif line.startswith('*MESH_FACE_LIST'):
                continue

            if line.startswith('*MESH_VERTEX'):
                parts = re.split(r'\s+', line.strip())
                vertices.append((float(parts[2]), float(parts[3]), float(parts[4])))
            elif line.startswith('*MESH_VERTCOL'):
                parts = re.split(r'\s+', line.strip())
                colors.append((float(parts[2]), float(parts[3]), float(parts[4])))
            elif line.startswith('*MESH_FACE'):
                parts = re.split(r'\s+', line.strip())
                face_indices = (int(parts[3]), int(parts[5]), int(parts[7]))
                faces.append(face_indices)

    return vertices, colors, faces

def export_vertex_data(vertices, colors, faces, output_path):
    with open(output_path, 'w') as file:
        for v, c in zip(vertices, colors):
            file.write(f'{v[0]} {v[1]} {v[2]} {c[0]} {c[1]} {c[2]}\n')
        for f in faces:
            file.write(f'f {f[0]+1} {f[1]+1} {f[2]+1}\n')

# 使用示例
vertices, colors, faces = parse_ase('Untitled.ase')
export_vertex_data(vertices, colors, faces, 'Untitled.txt')