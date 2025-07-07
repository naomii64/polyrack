class Mat4{
    static positions = [
        [0,1,2,3],
        [4,5,6,7],
        [8,9,10,11],
        [12,13,14,15]
    ];
    static identity = [
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1];
    static create(){return Mat4.identity.slice()}
    //creates a perspective matrix (#stolen hehehehe)
    static perspective(target,n, r, a, e){
        for(let i in target){target[i]=0;}
        const p=Mat4.positions;
        let u=1/Math.tan(n/2);
        let o=void 0;
        target[p[0][0]]=u/r;
        target[p[1][1]]=u;
        target[p[3][2]]=-1;
        if(null!=e&&e!==1/0){
            o=1/(a-e);
            target[p[2][2]]=(e+a)*o;
            target[p[2][3]]=2*e*a*o;
        }else{
            target[p[2][2]]=-1;
            target[p[2][3]]=-2*a;
        }
    }
    static translate(target,x,y,z){
        const p=Mat4.positions;
        const mat=[
            1,0,0,x,
            0,1,0,y,
            0,0,1,z,
            0,0,0,1
        ];
        Mat4.multiply(target,mat)
    }
    static scale(target,x,y,z){
        const m=[
            x,0,0,0,
            0,y,0,0,
            0,0,z,0,
            0,0,0,1,
        ]
        Mat4.multiply(target,m);
    }
    static rotate(target,x, y, z) {
        let cosX = Math.cos(x);
        let sinX = Math.sin(x);
        let cosY = Math.cos(y);
        let sinY = Math.sin(y);
        let cosZ = Math.cos(z);
        let sinZ = Math.sin(z);
        const rotationX = [
            1, 0, 0, 0,
            0, cosX, -sinX, 0,
            0, sinX, cosX, 0,
            0, 0, 0, 1
        ];
        // Apply Y rotation second (yaw)
        const rotationY = [
            cosY, 0, sinY, 0,
            0, 1, 0, 0,
            -sinY, 0, cosY, 0,
            0, 0, 0, 1
        ];
        // Apply Z rotation third (roll)
        const rotationZ= [
            cosZ, -sinZ, 0, 0,
            sinZ, cosZ, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ];
        // Combine the rotations: X -> Y -> Z
        Mat4.multiply(target, rotationZ);
        Mat4.multiply(target, rotationY);
        Mat4.multiply(target, rotationX);
    }
    static rotateINV(target,x, y, z) {
        let cosX = Math.cos(x);
        let sinX = Math.sin(x);
        let cosY = Math.cos(y);
        let sinY = Math.sin(y);
        let cosZ = Math.cos(-z);
        let sinZ = Math.sin(-z);
        const rotationX = [
            1, 0, 0, 0,
            0, cosX, -sinX, 0,
            0, sinX, cosX, 0,
            0, 0, 0, 1
        ];
        // Apply Y rotation second (yaw)
        const rotationY = [
            cosY, 0, sinY, 0,
            0, 1, 0, 0,
            -sinY, 0, cosY, 0,
            0, 0, 0, 1
        ];
        // Apply Z rotation third (roll)
        const rotationZ= [
            cosZ, -sinZ, 0, 0,
            sinZ, cosZ, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ];
        
        Mat4.multiply(target, rotationX);
        Mat4.multiply(target, rotationZ);
        Mat4.multiply(target, rotationY);


    }
    static multiply(A, B) {
        const original = A.slice();

        for (let row = 0; row < 4; row++) {
            for (let col = 0; col < 4; col++) {
                A[row * 4 + col] =
                    original[row * 4 + 0] * B[0 * 4 + col] +
                    original[row * 4 + 1] * B[1 * 4 + col] +
                    original[row * 4 + 2] * B[2 * 4 + col] +
                    original[row * 4 + 3] * B[3 * 4 + col];
            }
        }
    }
}
class Vec3{
    static INV_VECTOR_SIZE = 1/3;
    static abs(input){
        return [Math.abs(input[X]),Math.abs(input[Y]),Math.abs(input[Z])];
    }
    static sqrLength(input){
        return (input[X]**2)+(input[Y]**2)+(input[Z]**2);
    }
    static length(input){
        return Math.sqrt((input[X]**2)+(input[Y]**2)+(input[Z]**2));
    }
    static cheapDist(A,B,dist){
        const v = Vec3.abs(Vec3.sub(A,B));
        return ((v[X]<dist)&&(v[Y]<dist)&&(v[Z]<dist))
    }
    static normalize(input){
        return Vec3.mulf(input,1/Vec3.length(input));
    }
    static divf(input,float){
        return [input[X]/float,input[Y]/float,input[Z]/float];
    }
    static mulf(input,float){
        return input.map(v => v*float);
    }
    static subf(input,float){
        return [input[X]-float,input[Y]-float,input[Z]-float];
    }
    static addf(input,float){
        return [input[X]+float,input[Y]+float,input[Z]+float];
    }
    static add(A,B){
        return [A[X]+B[X],A[Y]+B[Y],A[Z]+B[Z]];
    }
    static sub(A,B){
        return [A[X]-B[X],A[Y]-B[Y],A[Z]-B[Z]];    
    }
    static addy(input,add){
        return [input[X],input[Y]+add,input[Z]];
    }
    static clamp(input,min,max){
        let returnVector = [input[X],input[Y],input[Z]];
        for(let i = 0;i<=2;i++){
            if(min[i]!=null&&max[i]!=null){
                returnVector[i] = Math.max(Math.min(returnVector[i],max[i]),min[i]);
            }
        }
        return returnVector;
    }
    static mul(A,B){
        return [A[X]*B[X],A[Y]*B[Y],A[Z]*B[Z]];
    }
    static floor(input){
        return input.map(v => Math.floor(v));
    }
    static round(input){
        return [Math.round(input[X]),Math.round(input[Y]),Math.round(input[Z])];
    }
    static cheapRotateY(input){
        return [-input[Z],input[Y],input[X]];
    }

    static cheapRotateX(input){
        return [input[X],input[Z],-input[Y]];
    }

    static cheapRotateZ(input){
        return [-input[Y],input[X],input[Z]];
    }
    static lerp(A,B,i){
        return[
            A[X]+((B[X]-A[X])*i),
            A[Y]+((B[Y]-A[Y])*i),
            A[Z]+((B[Z]-A[Z])*i)
        ]
    }
    static cheapRotateAxis(input,axis){
        switch (parseInt(axis)){
            case X:
                return Vec3.cheapRotateX(input);
            case Y:
                return Vec3.cheapRotateY(input);
            case Z:
                return Vec3.cheapRotateZ(input);
        }
    }
    static getDirectionFromRotation(input) {
        let x = Math.sin(input[Y])*Math.cos(input[X]);
        let y = -Math.sin(input[X]);
        let z = -Math.cos(input[Y])*Math.cos(input[X]);
    
        return [x, y, z];
    }
    static equals(A,B){
        if(A==null||B==null){return false;}
        return ((A[X]==B[X])&&(A[Y]==B[Y])&&(A[Z]==B[Z]));
    }
    static rotateY(input,angle) {
        const cosAngle = Math.cos(angle);
        const sinAngle = Math.sin(angle);
    
        const newX = input[X] * cosAngle - input[Z] * sinAngle;
        const newZ = input[X] * sinAngle + input[Z] * cosAngle;
    
        // Round the results to a reasonable precision (e.g., 6 decimal places)
        return [
            parseFloat(newX.toFixed(6)), 
            input[Y], 
            parseFloat(newZ.toFixed(6))
        ];
    }
    static sign(input){
        return Math.sign(input[X]+input[Y]+input[Z]);
    }
    static thirdAxis(a, b) {
        return [
            a[Y] * b[Z] - a[Z] * b[Y], // x
            a[Z] * b[X] - a[X] * b[Z], // y
            a[X] * b[Y] - a[Y] * b[X]  // z
        ];
    }
    static string(v){
        return `${v[X]},${v[Y]},${v[Z]}`;
    }
    static rotateX(input, angle) {
        const cosAngle = Math.cos(angle);
        const sinAngle = Math.sin(angle);
    
        const newY = input[Y] * cosAngle - input[Z] * sinAngle;
        const newZ = input[Y] * sinAngle + input[Z] * cosAngle;
    
        // Return the rotated vector
        return [
            input[X],  // X component remains unchanged
            parseFloat(newY.toFixed(6)),  // Y component is rotated
            parseFloat(newZ.toFixed(6))   // Z component is rotated
        ];
    }
}