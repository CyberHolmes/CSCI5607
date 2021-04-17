v = []
vt = []
vn = []
vp = []
face = []
# fileName = input('Enter file name: ')
fileName = 'models/ZombieSmooth.obj'
with open(fileName) as f:
    for line in f:
        temp = line.strip().split(' ')
        if temp[0]=='v':
            v.append(line.strip())
        elif temp[0]=='vt':
            vt.append(line.strip())
        elif temp[0]=='vn':
            vn.append(line.strip())
        elif temp[0]=='vp':
            vp.append(line.strip())
        elif temp[0]=='f':
            for i in range(2,len(temp)-1):
                face.append(temp[1])
                face.append(temp[i])
                face.append(temp[i+1])
new_content = []
for face_i in face:
    idx = face_i.split('/')
    #Get vertex
    cur_v = v[int(idx[0])-1].split(' ')
    for t in cur_v[1:]:
        new_content.append(t)
    if idx[1]=='': #no texture coordinates:
        new_content.append('0')
        new_content.append('0')
    else:
        cur_vt = vt[int(idx[1])-1].split(' ')
        for t in cur_vt[1:3]:
            new_content.append(t)
    cur_vn = vn[int(idx[2])-1].split(' ')
    for t in cur_vn[1:]:
        new_content.append(t)
temp = fileName.split('/')[-1].split('.')[0]
new_fileName = './models/'+temp+'.txt'
nf = open(new_fileName,"w")
nf.writelines(str(len(new_content))+'\n')
for nc in new_content:
    nf.writelines(nc+'\n')
