class TextureEditor {
    static init(){
        //get elements
        TextureEditor.textureViewCanvas=document.getElementById('texture-canvas');
        TextureEditor.ctx=TextureEditor.textureViewCanvas.getContext('2d');
        TextureEditor.list=document.getElementById('texture-list');
    }
    static handleFile(file) {
        const reader = new FileReader();
        reader.onload = (event) => {
            const img = new Image();
            img.onload = () => {
                const canvas = document.createElement("canvas");
                canvas.width = img.width;
                canvas.height = img.height;

                const ctx = canvas.getContext("2d");
                ctx.drawImage(img, 0, 0);

                const imageData = ctx.getImageData(0, 0, img.width, img.height);

                const result = {
                    width: img.width,
                    height: img.height,
                    pixels: imageData.data, // Uint8ClampedArray: [r, g, b, a, r, g, b, a, ...]
                    name:file.name.substring(0,file.name.length-4)
                };
                TE.addTexture(result);
            };
            img.src = event.target.result;
        };
        reader.readAsDataURL(file);
    }
    static addTexture(data){
        data.name=data.name;
        project.textures.push(data);
        const btn=document.createElement('button');
        TE.list.appendChild(btn);
        btn.classList.add('list-button');
        btn.textContent=data.name;
        btn.onclick=function(){
            TE.loadTexture(data);
            highlightButtonInList(this);
        }
    }
    static loadTexture(tex){
        if(tex.width>tex.height){
            TE.textureViewCanvas.style.width='100%'
            TE.textureViewCanvas.style.height=null;
        }else{
            TE.textureViewCanvas.style.width=null;
            TE.textureViewCanvas.style.height='100%';
        }
        TE.textureViewCanvas.width=tex.width;
        TE.textureViewCanvas.height=tex.height;
        //print out the image to the canvas
        const imageData = new ImageData(tex.pixels, tex.width, tex.height);
        TE.ctx.putImageData(imageData, 0, 0);
    }
    static reset(){
        project.textures=[];
        TE.list.innerHTML='';
    }
    static getBase64(){
        document.getElementById("image-base64-output").textContent=TE.textureViewCanvas.toDataURL('image/png');
    }
}
const TE=TextureEditor;