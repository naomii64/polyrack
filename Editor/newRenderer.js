class Meshes{
    static UVsphere(resLat=5,resLong=5,rad=1){
        const geometry={vertices:[],colors:[],tris:[]};
        //THESE ARE BACKWARDS FIX LATER
        for(let lat=0;lat<resLat;lat++){
            for(let long=1;long<resLong-1;long++){
                const cLat=(lat/resLat);
                const nLat=((lat+1)/resLat);
                const cLong=(long/resLong);
                const nLong=((long+1)/resLong);
                    
                const cRadLat=cLat*Math.PI*2;
                const nRadLat=nLat*Math.PI*2;
                const cRadLong=cLong*Math.PI;
                const nRadLong=nLong*Math.PI;

                const cr=Math.sin(cRadLong);
                const nr=Math.sin(nRadLong);
                
                const x=Math.cos(cRadLat);
                const z=Math.sin(cRadLat);
                const y=-Math.cos(cRadLong);

                const nx=Math.cos(nRadLat);
                const nz=Math.sin(nRadLat);
                const ny=-Math.cos(nRadLong);
            
                geometry.tris.push(...[0,1,2,3,0,2].map(v=>v+(geometry.vertices.length/3)));

                
                geometry.vertices.push(nx*cr,y,nz*cr);
                geometry.vertices.push(nx*nr,ny,nz*nr);
                geometry.vertices.push(x*nr,ny,z*nr);
                geometry.vertices.push(x*cr,y,z*cr);
            
                //make all white
                geometry.colors.push(...new Array(12).fill(1));
            }
            
        }
        return geometry;
    }
    static cube(size=1){
        //unfinished
        const face1=[
            [-1,-1,-1,   -1,0,0,   1,1,1,1, 0,0,],
            [-1,-1, 1,   -1,0,0,   1,1,1,1, 0,0,],
            [-1,1,1,   -1,0,0,   1,1,1,1, 0,0,],
            [-1,1,-1,   -1,0,0,   1,1,1,1, 0,0,],
        ];
        const face2=[
            [1,1,1,   1,0,0,   1,1,1,1, 0,0,],
            [1,-1, 1,   1,0,0,   1,1,1,1, 0,0,],
            [1,-1,-1,   1,0,0,   1,1,1,1, 0,0,],
            [1,1,-1,   1,0,0,   1,1,1,1, 0,0,],
        ];
        const face3=[
            [1,-1,1,   0,-1,0,   1,1,1,1, 0,0,],
            [-1,-1, 1,   0,-1,0,   1,1,1,1, 0,0,],
            [-1,-1,-1,   0,-1,0,   1,1,1,1, 0,0,],
            [1,-1,-1,   0,-1,0,   1,1,1,1, 0,0,],
        ];
        const face4=[
            [-1,1,-1,   0,1,0,   1,1,1,1, 0,0,],
            [-1,1, 1,   0,1,0,   1,1,1,1, 0,0,],
            [1,1,1,   0,1,0,   1,1,1,1, 0,0,],
            [1,1,-1,   0,1,0,   1,1,1,1, 0,0,],
        ];

        const face5=[
            [-1,-1,-1,   0,0,-1,   1,1,1,1, 0,0,],
            [-1, 1,-1,   0,0,-1,   1,1,1,1, 0,0,],
            [1,1,-1,   0,0,-1,   1,1,1,1, 0,0,],
            [1,-1,-1,   0,0,-1,   1,1,1,1, 0,0,],
        ];
        const face6=[
            [1,1,1,   0,0,1,   1,1,1,1, 0,0,],
            [-1, 1,1,   0,0,1,   1,1,1,1, 0,0,],
            [-1,-1,1,   0,0,1,   1,1,1,1, 0,0,],
            [1,-1,1,   0,0,1,   1,1,1,1, 0,0,],
        ];
        
        return new Float32Array([
            //left
            ...face1[0],
            ...face1[1],
            ...face1[2],
            ...face1[3],
            ...face1[0],
            ...face1[2],
            //right
            ...face2[0],
            ...face2[1],
            ...face2[2],
            ...face2[3],
            ...face2[0],
            ...face2[2],
            //bottom
            ...face3[0],
            ...face3[1],
            ...face3[2],
            ...face3[3],
            ...face3[0],
            ...face3[2],
            //top
            ...face4[0],
            ...face4[1],
            ...face4[2],
            ...face4[3],
            ...face4[0],
            ...face4[2],
            //front and back
            ...face5[0],
            ...face5[1],
            ...face5[2],
            ...face5[3],
            ...face5[0],
            ...face5[2],
            ...face6[0],
            ...face6[1],
            ...face6[2],
            ...face6[3],
            ...face6[0],
            ...face6[2],
        ]);
    }
    static arrow(length=4,res=5,ratio=0.5, coneLength=1){
        const cylLength=length-coneLength;
        
        let geometry=[];
        const p=Math.PI*2;
        for(let x=0;x<res;x++){
            const rx=x/res;
            const nrx=(x+1)/res;
            const points2d=[
                [Math.cos(rx*p),Math.sin(rx*p)],
                [Math.cos(nrx*p),Math.sin(nrx*p)],
            ];
            const points=[
                //cyl
                [points2d[1][X]*ratio,cylLength,points2d[1][Y]*ratio,
                 /*normals*/points2d[1][X],0,points2d[1][Y],/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[1][X]*ratio,0,points2d[1][Y]*ratio,/*normals*/points2d[1][X],0,points2d[1][Y],/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[0][X]*ratio,0,points2d[0][Y]*ratio,/*normals*/points2d[0][X],0,points2d[0][Y],/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[0][X]*ratio,cylLength,points2d[0][Y]*ratio,
                 /*normals*/points2d[0][X],0,points2d[0][Y],/*rgba*/1,1,1,1,/*uv*/0,0],
                //base
                [points2d[1][X]*ratio,0,points2d[1][Y]*ratio,/*normals*/0,-1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[0][X]*ratio,0,points2d[0][Y]*ratio,/*normals*/0,-1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                [0,0,0,/*normals*/0,-1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                //base of the cone
                [0,cylLength,0,/*normals*/0,-1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[0][X],cylLength,points2d[0][Y],/*normals*/0,-1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[1][X],cylLength,points2d[1][Y],/*normals*/0,-1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                //tip of the cone
                [0,length,0,/*normals*/0,1,0,/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[0][X],cylLength,points2d[0][Y],
                /*normals*/points2d[0][X],0,points2d[0][Y],/*rgba*/1,1,1,1,/*uv*/0,0],
                [points2d[1][X],cylLength,points2d[1][Y],
                 /*normals*/points2d[1][X],0,points2d[1][Y],/*rgba*/1,1,1,1,/*uv*/0,0],    
            ];
            //cilindar
            geometry.push(...points[0]);
            geometry.push(...points[1]);
            geometry.push(...points[2]);
            geometry.push(...points[3]);
            geometry.push(...points[0]);
            geometry.push(...points[2]);
            //base
            geometry.push(...points[4]);
            geometry.push(...points[6]);
            geometry.push(...points[5]);
            //base of cone
            geometry.push(...points[7]);
            geometry.push(...points[8]);
            geometry.push(...points[9]);
            //top of cone
            geometry.push(...points[10]);
            geometry.push(...points[12]);
            geometry.push(...points[11]);
            
        }
        
        return new Float32Array(geometry);
    }
    static setColors(m,r=255,g=255,b=255,a=255){
        const s=1/255;
        for(let i=0;i<m.length;i+=Renderer.VERTEX_SIZE){
            m[i+6]=r*s;
            m[i+6+1]=g*s;
            m[i+6+2]=b*s;
            m[i+6+3]=a*s;
        }
    }
    static swapVertexValues(m,valA,valB,mul=1){
        for(let i=0;i<m.length;i+=Renderer.VERTEX_SIZE){
            const a=m[i+valA];
            const b=m[i+valB];
            m[i+valA]=b*mul;
            m[i+valB]=a;
        }
    }
    static grid(width,height,unit=1,lineWidth=0.05){
        const data=[];
        const lwd2=lineWidth*0.5;

        for(let x=0;x<=width;x++){
            const ux=x*unit;
            const positions=[
            [ux-lwd2,-lwd2,0],
            [ux-lwd2,height+lwd2,0],
            [ux+lwd2,height+lwd2,0],
            [ux+lwd2,-lwd2,0]
            ];
            //frontFace
            data.push(...positions[0],  1,1,1,  1,1,1,1, 0,0);
            data.push(...positions[1], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[2], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[3], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[0], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[2], 1,1,1,   1,1,1,1, 0,0);
            //backface
            data.push(...positions[2],  1,1,1,  1,1,1,1, 0,0);
            data.push(...positions[1], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[0], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[3], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[2], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[0], 1,1,1,   1,1,1,1, 0,0);
        }

        for(let y=0;y<=height;y++){
            const uy=y*unit;
            const positions=[
            [-lwd2,uy-lwd2,0],
            [width+lwd2,uy-lwd2,0],
            [width+lwd2,uy+lwd2,0],
            [-lwd2,uy+lwd2,0]
            ];
            //frontFace
            data.push(...positions[0],  1,1,1,  1,1,1,1, 0,0);
            data.push(...positions[1], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[2], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[3], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[0], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[2], 1,1,1,   1,1,1,1, 0,0);
            //backface
            data.push(...positions[2],  1,1,1,  1,1,1,1, 0,0);
            data.push(...positions[1], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[0], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[3], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[2], 1,1,1,   1,1,1,1, 0,0);
            data.push(...positions[0], 1,1,1,   1,1,1,1, 0,0);
        }
        return new Float32Array(data);
    }
}

const X=0,Y=1,Z=2,NX=3,NY=4,NZ=5,R=6,G=7,B=8;
    const vsSource = `#version 300 es
in vec4 aVertexPosition;
in vec4 aVertexColor;
in vec3 aVertexNormal;
in vec2 aVertexUV;

uniform mat4 uProjection;
uniform mat4 uModelSpace;
uniform mat4 uView;
uniform mat4 uNormal;

out vec4 vColor;
out vec3 vNormal;
out vec2 vUV;

void main() {
    vec4 projected = aVertexPosition * uModelSpace * uView * uProjection;
    gl_Position = projected;
    
    vColor = aVertexColor;
    vUV = aVertexUV;
    vNormal = (uNormal * vec4(aVertexNormal, 0.0)).xyz;
}
  `;
class Renderer{
    static fsSource = `#version 300 es
precision mediump float;

in vec4 vColor;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uTexture;

out vec4 fragColor;

float ditherPattern(vec2 pos) {
    float bayer[16] = float[](
        0.0, 8.0, 2.0, 10.0,
        12.0, 4.0, 14.0, 6.0,
        3.0, 11.0, 1.0, 9.0,
        15.0, 7.0, 13.0, 5.0
    );

    int x = int(floor(fract(pos.x) * 4.0));
    int y = int(floor(fract(pos.y) * 4.0));

    return bayer[y * 4 + x]/16.0;
}

vec4 dither(vec4 color){
    vec4 colorA = floor(color * 16.0) / 16.0;
    vec4 colorB = ceil(color * 16.0) / 16.0;

    vec4 colorDiff = colorB - colorA;
    colorDiff = max(colorDiff, vec4(1e-6)); // avoid division by zero

    float ditherSize = 4.0;
    vec2 ditherPos = fract(gl_FragCoord.xy / ditherSize);

    vec4 dith = vec4(round(ditherPattern(ditherPos)*4.0)/4.0);
    vec4 colorAdifference = (color - colorA) / colorDiff;
    dith = dith + colorAdifference;

    return mix(colorA, colorB, floor(dith));
}

void main() {
    vec4 textureColor = texture(uTexture, vUV);
    vec3 unitNormal = (vNormal + vec3(1.0)) * 0.5;
    vec4 lighting = vec4(vec3(unitNormal.y + (unitNormal.x * 0.5)), 1.0);
    fragColor = dither(textureColor * lighting * vColor);
}`;
    

    
    constructor(x,y){
        this.canvas=document.createElement('canvas');
        this.canvas.width=x;
        this.canvas.height=y;
        this.gl=this.canvas.getContext("webgl2", { antialias: false });
        this.aspect=this.canvas.height/this.canvas.width;
        this.shaderProgram=this.initShaderProgram(this.gl,vsSource,Renderer.fsSource);
        this.meshes=[];
        this.gl.enable(this.gl.CULL_FACE);
        this.gl.cullFace(this.gl.BACK);
        
        this.gl.clearDepth(1.0);
        this.gl.depthFunc(this.gl.LEQUAL);
        this.gl.enable(this.gl.DEPTH_TEST);

        this.viewMatrix=Mat4.create();
        this.cam={};
        this.cam.pos={x:0,y:0,z:0};
        this.cam.rot={x:0,y:0,z:0};
        //default stuff
        this.defaultTexture=this.createGLTexture({
            height:1,
            width:1,
            pixels:new Uint8Array([255,255,255,255])
        });
        const arrowThinness=16;
        const brightener=70;
        const coneLength=2;
        const res=6;
        const ratio=0.5;
        //create axis
        const zArrow=Meshes.arrow(arrowThinness,res,ratio,coneLength);
        Meshes.setColors(zArrow,brightener,brightener,255);
        const yArrow=Meshes.arrow(arrowThinness,res,ratio,coneLength);
        Meshes.setColors(yArrow,brightener,255,brightener);
        const xArrow=Meshes.arrow(arrowThinness,res,ratio,coneLength);
        Meshes.setColors(xArrow,255,brightener,brightener);
        
        //rotate x and z
        Meshes.swapVertexValues(zArrow,Y,Z,-1);
        Meshes.swapVertexValues(xArrow,Y,X,-1);
        //join meshes together
        const finalArray=new Float32Array(xArrow.length*3);
        finalArray.set(xArrow,0);
        finalArray.set(yArrow,xArrow.length);
        finalArray.set(zArrow,xArrow.length*2);
        
        this.axis=this.newMeshRaw(finalArray);
        this.axis.scale.fill(1/arrowThinness);
    }
    drawAxis(){
        this.drawMesh(this.axis);
    }
    setViewPosition(x,y,z){
        this.cam.pos.x=x;
        this.cam.pos.y=y;
        this.cam.pos.z=z;
        this.recalculateView();
    }
    setViewRotation(x, y, z) {
        this.cam.rot.x = x;
        this.cam.rot.y = y;
        this.cam.rot.z = z;
        this.recalculateView();
    }
    recalculateView() {
        this.viewMatrix = Mat4.create();


        Mat4.rotateINV(this.viewMatrix,this.cam.rot.x,this.cam.rot.y,this.cam.rot.z);
        Mat4.translate(this.viewMatrix, -this.cam.pos.x, -this.cam.pos.y, -this.cam.pos.z);
    }
    createGLTexture(texData) {
        const gl = this.gl;

        const texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, texture);

        // Upload pixel data
        gl.texImage2D(
            gl.TEXTURE_2D,
            0,                 // mip level
            gl.RGBA,           // internal format
            texData.width,
            texData.height,
            0,                 // border
            gl.RGBA,           // source format
            gl.UNSIGNED_BYTE,  // source type
            texData.pixels
        );

        // Set parameters
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST); // or gl.LINEAR
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

        gl.bindTexture(gl.TEXTURE_2D, null);

        return texture;
    }
    initShaderProgram(gl, vsSource, fsSource) {
        const vertexShader = this.loadShader(gl, gl.VERTEX_SHADER, vsSource);
        const fragmentShader = this.loadShader(gl, gl.FRAGMENT_SHADER, fsSource);
        // Create the shader program
        const shaderProgram = gl.createProgram();
        gl.attachShader(shaderProgram, vertexShader);
        gl.attachShader(shaderProgram, fragmentShader);
        gl.linkProgram(shaderProgram);
        
        if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        alert(
        `Unable to initialize the shader program: ${gl.getProgramInfoLog(
            shaderProgram,
        )}`,
        );
    return null;
  }

  return shaderProgram;
}
    setShaderProgram(vsSource, fsSource) {
    const gl = this.gl;
    const vertexShader = this.loadShader(gl, gl.VERTEX_SHADER, vsSource);
    const fragmentShader = this.loadShader(gl, gl.FRAGMENT_SHADER, fsSource);

    const newProgram = gl.createProgram();
    gl.attachShader(newProgram, vertexShader);
    gl.attachShader(newProgram, fragmentShader);
    gl.linkProgram(newProgram);

    if (!gl.getProgramParameter(newProgram, gl.LINK_STATUS)) {
        console.error('Could not link shader program:', gl.getProgramInfoLog(newProgram));
        return;
    }

    this.shaderProgram = newProgram;
    }
    loadShader(gl, type, source) {
        const shader = gl.createShader(type);
        gl.shaderSource(shader, source);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert(
        `An error occurred compiling the shaders: ${gl.getShaderInfoLog(shader)}`,
        );
        gl.deleteShader(shader);
            return null;
        }

        return shader;
    }
        createPerspective(fov, znear = 0.1, zfar = 1000) {
            this.projectionMatrix = Mat4.create();
            Mat4.perspective(this.projectionMatrix, fov * Math.PI / 180, 1 / this.aspect, znear, zfar);

            this.gl.useProgram(this.shaderProgram); // <== Ensure shader is active
            this.gl.uniformMatrix4fv(
                this.gl.getUniformLocation(this.shaderProgram, 'uProjection'),
                false,
                this.projectionMatrix
            );
        }
        draw(){
            const gl=this.gl;
            gl.clearColor(0.5, 0.5, 0.5, 1.0);
            gl.clear(gl.COLOR_BUFFER_BIT);
            gl.clear(gl.DEPTH_BUFFER_BIT);
            
            for(let v of this.meshes){this.drawMesh(v)}
        }
        drawMesh(mesh) {
            const gl = this.gl;
            const mPos = mesh.position;
            const mRot = mesh.rotation;
            const mScale = mesh.scale;
            const modelMat = Mat4.create();
            Mat4.translate(modelMat, mPos[X], mPos[Y], mPos[Z]);
            Mat4.rotate(modelMat, mRot[X], mRot[Y], mRot[Z]);
            Mat4.scale(modelMat,mScale[X],mScale[Y],mScale[Z]);
        
            const modelNormalMatrix=Mat4.create();
            Mat4.rotateINV(modelNormalMatrix,mRot[X], mRot[Y], mRot[Z]);

            gl.useProgram(this.shaderProgram);
        
            const vpos = gl.getAttribLocation(this.shaderProgram, "aVertexPosition");
            const cpos = gl.getAttribLocation(this.shaderProgram, "aVertexColor");
            const npos = gl.getAttribLocation(this.shaderProgram, "aVertexNormal");
            const upos = gl.getAttribLocation(this.shaderProgram, "aVertexUV");
        
            gl.uniformMatrix4fv(gl.getUniformLocation(this.shaderProgram, 'uModelSpace'), false, modelMat);
            gl.uniformMatrix4fv(gl.getUniformLocation(this.shaderProgram, 'uProjection'), false, this.projectionMatrix);
            gl.uniformMatrix4fv(gl.getUniformLocation(this.shaderProgram, 'uView'), false, this.viewMatrix)
            gl.uniformMatrix4fv(gl.getUniformLocation(this.shaderProgram, 'uNormal'), false, modelNormalMatrix)

            const uTexLoc = gl.getUniformLocation(this.shaderProgram, "uTexture");

            let tex=mesh.texture;
            if (!tex) {
                tex=this.defaultTexture;
            }

            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, tex);
            gl.uniform1i(uTexLoc, 0); // texture unit 0

            const stride = Float32Array.BYTES_PER_ELEMENT * Renderer.VERTEX_SIZE;
        
            gl.bindBuffer(gl.ARRAY_BUFFER, mesh.buffer);
            
            gl.enableVertexAttribArray(vpos);
            gl.enableVertexAttribArray(npos);
            gl.enableVertexAttribArray(cpos);
            gl.enableVertexAttribArray(upos);
        
            gl.vertexAttribPointer(vpos, 3, gl.FLOAT, false, stride, 0);
            gl.vertexAttribPointer(npos, 3, gl.FLOAT, false, stride, Float32Array.BYTES_PER_ELEMENT * 3);
            gl.vertexAttribPointer(cpos, 4, gl.FLOAT, false, stride, Float32Array.BYTES_PER_ELEMENT * 6);
            gl.vertexAttribPointer(upos, 2, gl.FLOAT, false, stride, Float32Array.BYTES_PER_ELEMENT * 10);
        
            gl.drawArrays(gl.TRIANGLES, 0, mesh.vertexCount);
        
            gl.disableVertexAttribArray(vpos);
            gl.disableVertexAttribArray(npos);
            gl.disableVertexAttribArray(cpos);
            gl.disableVertexAttribArray(upos);
        }
        static VERTEX_SIZE=12;
        newMesh(geometry){
            const gl=this.gl;
            //assemble into one array
            const triLength=3;
            const finalGeometry=new Float32Array(Renderer.VERTEX_SIZE*geometry.tris.length);
            for(let i=0;i<geometry.tris.length;i++){
                const v=geometry.tris[i];
                const j=i*Renderer.VERTEX_SIZE;
                finalGeometry[j+X]=geometry.vertices[(v*3)+X];
                finalGeometry[j+Y]=geometry.vertices[(v*3)+Y];
                finalGeometry[j+Z]=geometry.vertices[(v*3)+Z];
                finalGeometry[j+R]=geometry.colors[(v*3)+X];
                finalGeometry[j+G]=geometry.colors[(v*3)+Y];
                finalGeometry[j+B]=geometry.colors[(v*3)+Z];   
            }
            const buffer= gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
            this.calculateNormals(finalGeometry);
            gl.bufferData(gl.ARRAY_BUFFER, finalGeometry, gl.STATIC_DRAW);
            const mesh={
                buffer:buffer,
                vertexCount:geometry.tris.length,
                position:new Float32Array([0,0,0]),
                rotation:new Float32Array([0,0,0]),
                scale:new Float32Array([1,1,1]),
            }
            this.meshes.push(mesh);
            return mesh;
        }
        newMeshRaw(geometry){
            const gl=this.gl;
            const buffer= gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
            gl.bufferData(gl.ARRAY_BUFFER, geometry, gl.STATIC_DRAW);
            const mesh={
                buffer:buffer,
                vertexCount:geometry.length/Renderer.VERTEX_SIZE,
                position:new Float32Array([0,0,0]),
                rotation:new Float32Array([0,0,0]),
                scale:new Float32Array([1,1,1]),
                
                color:new Float32Array([1,1,1]),
                uvOffset:new Float32Array([0,0]),
            }
            this.meshes.push(mesh);
            return mesh;
        }
        setDragCenter(x,y,z){
            this.dragCenter=[x,y,z];
            this.updateCamPos();
        }
        makeDraggable(){
            this.dragCenter=[0,0,0];

            this.mouseDown=false;
            this.zoom=5;
            
            const renderer=this;
            document.addEventListener('mouseup',e=>{
                renderer.mouseDown=false;
            });
            this.canvas.addEventListener('mousedown',e=>{
                this.prev
                renderer.mouseDown=true;
            });
            this.updateCamPos=function(){
                const rotx=renderer.cam.rot.x;
                const roty=renderer.cam.rot.y;
                const rotz=renderer.cam.rot.z;
                const dist=renderer.zoom;    
                const dir=Vec3.getDirectionFromRotation([rotx,roty,rotz]);
                renderer.setViewPosition(
                    (-dir[X]*dist)+this.dragCenter[X],
                    (-dir[Y]*dist)+this.dragCenter[Y],
                    (-dir[Z]*dist)+this.dragCenter[Z]);
            }
            this.updateCamPos();
            this.addCamRotation=function(x,y,z){
                const rotx=clamp(renderer.cam.rot.x+x,-Math.PI/2,Math.PI/2);
                const roty=renderer.cam.rot.y+y;
                const rotz=renderer.cam.rot.z+z;    
                renderer.setViewRotation(rotx,roty,rotz);
            }
            
            this.canvas.addEventListener('mousemove',e=>{
                if(renderer.mouseDown){
                    const turnSpeed=1/100;
                    const deltaX=e.movementX*turnSpeed;
                    const deltaY=e.movementY*turnSpeed;
                    
                    renderer.addCamRotation(deltaY,deltaX,0);
                    
                    renderer.updateCamPos();
                }
            });
            document.addEventListener('wheel',e=>{
                if(renderer.hovered){
                    const zoomSpeed=1/100;
                    renderer.zoom+=e.deltaY*zoomSpeed;
                    document.body.scroll(0,0);
                    renderer.updateCamPos();
                }
            });
            

            this.canvas.addEventListener('mouseenter',e=>{
                renderer.hovered=true;
            });
            this.canvas.addEventListener('mouseleave',e=>{
                renderer.hovered=false;
            });
            const zoomAmnt=0.25;
            const keyRot=1/16;
            document.addEventListener('keydown',e=>{
                if(renderer.hovered){
                    const mul=(e.ctrlKey?2:1)
                    if(e.code=='ArrowUp'){
                        if(e.shiftKey){
                            e.preventDefault();
                            renderer.zoom-=zoomAmnt*mul;
                            renderer.updateCamPos();
                        }else{
                            e.preventDefault();
                            renderer.addCamRotation(-Math.PI*keyRot*mul,0,0);
                            renderer.updateCamPos();
                        }
                    }
                    if(e.code=='ArrowDown'){
                        if(e.shiftKey){
                            e.preventDefault();
                            renderer.zoom+=zoomAmnt*mul;
                            renderer.updateCamPos();
                        }else{
                            e.preventDefault();
                            renderer.addCamRotation(Math.PI*keyRot*mul,0,0);
                            renderer.updateCamPos();
                        }
                    }
                    if(e.code=='ArrowRight'){
                        e.preventDefault();
                        renderer.addCamRotation(0,Math.PI*keyRot*mul,0);
                        renderer.updateCamPos();
                    }
                    if(e.code=='ArrowLeft'){
                        e.preventDefault();
                        renderer.addCamRotation(0,-Math.PI*keyRot*mul,0);
                        renderer.updateCamPos();
                    }
                }
            });
        }
        calculateNormals(geometry){
            //for each tri
            for(let i=0;i<geometry.length;i+=Renderer.VERTEX_SIZE*3){
                const v1p=i;
                const v2p=v1p+Renderer.VERTEX_SIZE;
                const v3p=v2p+Renderer.VERTEX_SIZE;
                const v1=[geometry[v1p+X],geometry[v1p+Y],geometry[v1p+Z]];
                const v2=[geometry[v2p+X],geometry[v2p+Y],geometry[v2p+Z]];
                const v3=[geometry[v3p+X],geometry[v3p+Y],geometry[v3p+Z]];
                //get the cross product
                const u=Vec3.sub(v2,v1);
                const v=Vec3.sub(v3,v1);
                const n=Vec3.normalize(Vec3.mul(u,v));
                geometry[v1p+NX]=n[X];
                geometry[v2p+NX]=n[X];
                geometry[v3p+NX]=n[X];
                
                geometry[v1p+NY]=n[Y];
                geometry[v2p+NY]=n[Y];
                geometry[v3p+NY]=n[Y];
                
                geometry[v1p+NZ]=n[Z];
                geometry[v2p+NZ]=n[Z];
                geometry[v3p+NZ]=n[Z];
            }
        }
    }