import sys, getopt, functools, numpy

def main(argv):
    input_file = ''
    output_file = 'output.txt'
    try:
        opts, args = getopt.getopt(argv, "i:o:", ["input_file=", "output_file="])
    except getopt.GetoptError:
        print('obj_convert.py -i <input_file> -o <output_file>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-i':
            input_file = arg
        elif opt == '-o':
            output_file = arg
    if input_file != '':
        convert(input_file, output_file)
        print("Wrote " + input_file + " to " + output_file)

def convert(in_file_name, out_file_name):
    in_fs = open(in_file_name, 'r')
    obj_data = in_fs.read().split('\n')
    out_fs = open(out_file_name, 'w')
    out_fs.write("max_vertices: " + str(len(list(filter(lambda x: "v " in x, obj_data)))) + '\n')
    out_fs.write("max_normals: " + str(len(list(filter(lambda x: "vn " in x, obj_data)))) + '\n')
    verts = list(filter(lambda x: "v " in x, obj_data))
    pts = list(map(lambda x: x.split()[1:], verts))
    x_s = list(map(lambda x: float(x[0]), pts))
    y_s = list(map(lambda x: float(x[1]), pts))
    z_s = list(map(lambda x: float(x[2]), pts))
    print(len(x_s))
    min_x = min(x_s)
    max_x = max(x_s)
    min_y = min(y_s)
    max_y = max(y_s)
    min_z = min(z_s)
    max_z = max(z_s)
    out_fs.write("#Scene Dimensions: ("+str(min_x)+", "+str(min_y)+", "+str(min_z)+") -> ("+str(max_x)+", "+str(max_y)+", "+str(max_z)+")\n")
    materials = {}
    for line in obj_data:
        line_data = line.split()
        if len(line_data) == 0:
            continue
        # check what type of line this is: vertex, uv coordinate, normal, or material
        if line_data[0] == 'mtllib':
            # there is a material library we need to load
            materials = loadMaterialFile(line_data[1])
        elif line_data[0] == 'v':
            # this is a vertex
            vertex_str = "vertex: " + functools.reduce(lambda x, y: x + ' ' + y, line_data[1:])
            out_fs.write(vertex_str + '\n')
        elif line_data[0] == 'vt':
            uv_str = "uv: " + functools.reduce(lambda x, y: x + ' ' + y, line_data[1:])
            out_fs.write(uv_str + '\n')
        elif line_data[0] == 'vn':
            norm_str = "normal: " + functools.reduce(lambda x, y: x + ' ' + y, line_data[1:])
            out_fs.write(norm_str + '\n')
        elif line_data[0] == 'usemtl':
            # write material
            out_fs.write('#material--' + line_data[1] + '\n')
            mat_str = "material: " + materials[line_data[1]].writeToString()
            out_fs.write(mat_str + '\n')
            if materials[line_data[1]].texture_name != '':
                texture_str = "texture: " + materials[line_data[1]].texture_name
                out_fs.write(texture_str + '\n')
        elif line_data[0] == 'f':
            # this is where the lion's share of the work is done.
            # We need to triangulate the face AND determine whether it
            # is a triangle or normal_triangle
            f_verts = line_data[1:]
            attribs = [[], [] , []]
            for f_vert in f_verts:
                # split on the / to get the verts, tex_coords, and norms indices
                attrib_indices = f_vert.split('/')
                # the vertex is mandatory, so we will grab it without error checking
                # NOTE obj files are 1 indexed and we are using 0 indexing, hence the minus 1!
                n = 0
                while n < len(attrib_indices):
                    if attrib_indices[n] != "":
                        attribs[n].append(int(attrib_indices[n]) - 1)
                    else:
                        attribs[n].append(-1)
                    n = n + 1
                while n < 3:
                    # these are fluff values to keep everything aligned
                    attribs[n].append(-1)
                    n = n + 1
            # now every point on the face is in the attribs arrays
            # so all that is left to triangulate and write
            # we shall make the face into a triangle fan where the first point
            # is the fan center
            n = 2
            while n < len(attribs[0]):
                # first, check for normals. If even 1 normal is missing, this becomes a regular triangle
                triangle_type = ''
                verts = [attribs[0][0], attribs[0][n-1], attribs[0][n]]
                norms = list(filter(lambda x: int(x) >= 0, [attribs[2][0], attribs[2][n-1], attribs[2][n]]))
                uvs = list(filter(lambda x: x != '' and int(x) >= 0, [attribs[1][0], attribs[1][n-1], attribs[1][n]]))
                if len(norms) < 3:
                    triangle_type = "triangle: "
                else:
                    triangle_type = "normal_triangle: "
                triangle_type += functools.reduce(lambda x, y: str(x) + ' ' + str(y), verts)
                if len(norms) == 3:
                    triangle_type += ' ' + functools.reduce(lambda x, y: str(x) + ' ' + str(y), norms)
                if len(uvs) == 3:
                    triangle_type += ' ' + functools.reduce(lambda x, y: str(x) + ' ' + str(y), uvs)
                out_fs.write(triangle_type + '\n')
                n = n + 1
    out_fs.close()
    in_fs.close()

class material:
    def __init__(self):
        self.name = ''
        self.ka_r = 0.0
        self.ka_g = 0.0
        self.ka_b = 0.0
        self.kd_r = 0.0
        self.kd_g = 0.0
        self.kd_b = 0.0
        self.ks_r = 0.0
        self.ks_b = 0.0
        self.ks_g = 0.0
        self.ns = 0.0
        self.kt_r = 0.0
        self.kt_g = 0.0
        self.kt_b = 0.0
        self.ior = 0.0
        self.texture_name = ''

    def writeToString(self):
        # encode all the material data in a string
        mat_str = ''
        mat_str += str(self.ka_r) + ' ' + str(self.ka_g) + ' ' + str(self.ka_b) + ' '
        mat_str += str(self.kd_r) + ' ' + str(self.kd_g) + ' ' + str(self.kd_b) + ' '
        mat_str += str(self.ks_r) + ' ' + str(self.ks_g) + ' ' + str(self.ks_b) + ' ' + str(self.ns)
        mat_str += str(self.kt_r) + ' ' + str(self.kt_g) + ' ' + str(self.kt_b) + ' ' + str(self.ior)
        return mat_str

def loadMaterialFile(material_file_name):
    in_fs = open(material_file_name, 'r')
    file_data = in_fs.read().split('\n')
    # build a material string
    mats = {}
    m = material()
    name = ""
    for line in file_data:
        # read line and get type
        line_data = line.split()
        if line == '':
            continue
        if line_data[0] == 'newmtl':
            # this is a new material, save the old one
            mats[name] = m
            name = line_data[1]
            m = material()
        elif line_data[0] == 'Ka':
            # add ambiance
            m.ka_r = line_data[1]
            m.ka_g = line_data[2]
            m.ka_b = line_data[3]
        elif line_data[0] == 'Kd':
            # add ambiance
            m.kd_r = line_data[1]
            m.kd_g = line_data[2]
            m.kd_b = line_data[3]
        elif line_data[0] == 'Ks':
            # add ambiance
            m.ks_r = line_data[1]
            m.ks_g = line_data[2]
            m.ks_b = line_data[3]
        elif line_data[0] == 'Ns':
            # add specular highlight size
            m.ns = line_data[1]
        elif line_data[0] == 'Ni':
            # add Index of Refraction
            m.ior = line_data[1]   
        elif line_data[0] == 'map_Kd':
            # add texture
            m.texture_name = line_data[1]
    # grab the last material
    mats[name] = m
    in_fs.close()
    return mats

if __name__ == "__main__":
    main(sys.argv[1:])    