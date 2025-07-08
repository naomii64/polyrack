class Importer{
    static dropZone = document.getElementById("import-zone");
    static fileInput = document.getElementById("import-file-input");
    static init(){
        Importer.dropZone.addEventListener("dragover", function(e) {e.preventDefault();});
        Importer.dropZone.addEventListener("drop", function(e) {
            e.preventDefault();
            for(v of e.dataTransfer.files){
                const n=v.name;
                if(n.endsWith('.obj')){
                    Importer.handleFile(v);
                }
                if(n.endsWith('.png')){
                    TextureEditor.handleFile(v);
                }
                if(n.endsWith('.plk')){
                    loadProject(v);
                }
            }
            Importer.close();
        });
        Importer.close();
    }
    static open(){
        Importer.dropZone.style.display=null;
    }
    static close(){
        Importer.dropZone.style.display='none';
    }
}