class AnimationEditor{
    static currentAnim=null;
    static currentAnimInstance=null;

    static addAnimation(name='animation'){
        const animation={}
        project.animations.push(animation);
        animation.models=[];
        animation.name=name;
        const button=document.createElement('button');
        AE.list.appendChild(button);
        button.textContent=animation.name
        button.classList.add('list-button')
        button.onclick=function(){
            highlightButtonInList(this)
            AE.loadAnimation(animation);
        }
        button.onclick();
        return animation;
    }
    static init(){
        //get elements
        AE.previewSlider=document.getElementById('animation-preview-slider')
        AE.list=document.getElementById('animation-list');
        AE.modelList=document.getElementById('animation-model-list');
        AE.modelInputs=document.getElementById('animation-model-settings');
        AE.canvDiv=document.getElementById('animation-canvas-div');
        AE.animSettings=document.getElementById('animation-settings');
        //setup renderer
        AE.renderer=GLOBAL.createView();
        AE.canvDiv.appendChild(AE.renderer.canvas);

        AE.canvDiv.style.flexGrow=1;
        AE.canvDiv.style.height='100%';
    }
    static animate(){
        AE.renderer.stretchToParent();
        AE.renderer.drawAxis();
        if(AE.currentAnimInstance==null){return;}
        AE.currentAnimInstance.input.x=AE.previewSlider.value;
        AE.currentAnimInstance.draw(AE.renderer);
    }
    static loadAnimation(anim){
        AE.currentAnim=anim;
        AE.modelList.innerHTML='';
        AE.animSettings.innerHTML='';
        if(anim.hitboxPos==null){anim.hitboxPos=[0,0,0]}
        if(anim.hitboxSize==null){anim.hitboxSize=[0.5,0.5,0.5]}
        const animSettingsTable=UIHelper.quickTable();

        const hbp=UIHelper.fixedLengthArrayInput(anim.hitboxPos,'number');
        const hbs=UIHelper.fixedLengthArrayInput(anim.hitboxSize,'number');
        animSettingsTable.addText('hitbox position:');
        animSettingsTable.newRow();
        animSettingsTable.addElm(hbp);
        animSettingsTable.newRow();
        animSettingsTable.addText('hitbox size:');
        animSettingsTable.newRow();
        animSettingsTable.addElm(hbs);
        



        AE.animSettings.appendChild(animSettingsTable);
        

        for(let i in anim.models){
            const button=document.createElement('button');
            button.textContent=`model ${i}`;
            button.classList.add('list-button')
            const currentModel=anim.models[i];
            AE.modelList.appendChild(button);
            button.onclick=function(){
                highlightButtonInList(this);
                AE.selectAnimationModel(currentModel);     
            }
        }
        AE.currentAnimInstance=AnimationModel.from(anim,AE.renderer);
    }
    static selectAnimationModel(model){
        AE.modelInputs.innerHTML='';
        const modelIDinput=document.createElement('input');
        modelIDinput.type='Number';
        modelIDinput.min=0;
        modelIDinput.max=project.models.length-1;
        modelIDinput.step=1;
        modelIDinput.onchange=function(){
            model.modelID=this.value;
            AE.loadAnimation(AE.currentAnim);    
        }
        modelIDinput.value=model.modelID;

        const modelCountInput=document.createElement('input');
        modelCountInput.type='Number';
        modelCountInput.min=1;
        modelCountInput.step=1;
        modelCountInput.onchange=function(){
            model.count=this.value;
            AE.loadAnimation(AE.currentAnim);    
        }
        modelCountInput.value=model.count;

        
        AE.modelInputs.appendChild(modelIDinput)
        AE.modelInputs.appendChild(modelCountInput)

        
        const addLabel=function(text){
            const div=document.createElement('div');
            const ruleDiv=document.createElement('div');
            ruleDiv.innerHTML='<hr style="width:100%;">'
            ruleDiv.style.flexGrow=1;
            div.style.display='flex';
            const label=document.createElement('div');
            div.appendChild(label);            
            label.style.flexGrow=0;
            div.style.gap='4px';
            label.textContent=text;
            div.appendChild(label);
            div.style.width='100%'
            div.appendChild(ruleDiv)
            AE.modelInputs.appendChild(div);
        }
        const addParamInput=function(paramKey,paramLabel){
            const div=document.createElement('div');
            const input=document.createElement('input');
            const inputSubDiv=document.createElement('div');
            const labelDiv=document.createElement('div');
            labelDiv.textContent=paramLabel;
            

            input.style.display='inline';
            
            input.style.flexGrow=1;
            input.style.minWidth=0;
            inputSubDiv.style.display='flex';

            inputSubDiv.appendChild(input);
            div.appendChild(labelDiv);
            div.appendChild(inputSubDiv);
            div.style.display='flex';
            labelDiv.style.flexGrow=1;
            labelDiv.classList.add('label');
            inputSubDiv.style.width='75%';
            div.style.width='100%'
            AE.modelInputs.appendChild(div);

            //wory around the actual input stuff now
            input.value=model.functions[paramKey];
            
            input.onchange=function(){
                model.functions[paramKey]=this.value;
            }
        }
        addLabel('position');
        addParamInput('x','X');
        addParamInput('y','Y');
        addParamInput('z','Z');
        
        addLabel('rotation');
        addParamInput('rx','X');
        addParamInput('ry','Y');
        addParamInput('rz','Z');
        
        addLabel('scale');
        addParamInput('sx','X');
        addParamInput('sy','Y');
        addParamInput('sz','Z');
    }
    static scene=[];
    static addModel(){
        const mdl={};

        mdl.modelID=0;
        mdl.count=1;
        mdl.name='model';

        mdl.functions={
            ['x']:'0',
            ['y']:'0',
            ['z']:'0',

            ['rx']:'0',
            ['ry']:'0',
            ['rz']:'0',

            ['sx']:'1',
            ['sy']:'1',
            ['sz']:'1',
        }

        AE.currentAnim.models.push(mdl);
        AE.loadAnimation(AE.currentAnim);
    }
    static reset(){
        project.animations=[];
        AE.list.innerHTML='';
        AE.modelList.innerHTML='';
        AE.modelInputs.innerHTML='';
        AE.currentAnim=null;
    }
}
const AE=AnimationEditor;

class AnimationModel{
    constructor(){
        this.input={x:0,y:0};
    }
    static from(fromAnim,renderer){
        const anim=new AnimationModel();
        anim.models=[];
        anim.position=[0,0,0];
        anim.hitbox={};
        anim.hitbox.pos=fromAnim.hitboxPos;
        anim.hitbox.siz=fromAnim.hitboxSize;
        
        //wireframe cube
        anim.hitBoxWireFrame=renderer.newWireFrameRaw(new Float32Array([
            -1,1,1,  1,1,1,  1,1,1,1,    0,0,
            1,1,1,  1,1,1,  1,1,1,1,    0,0,

            -1,-1,1,  1,1,1,  1,1,1,1,    0,0,
            1,-1,1,  1,1,1,  1,1,1,1,    0,0,

            -1,1,1,  1,1,1,  1,1,1,1,    0,0,
            -1,-1,1,  1,1,1,  1,1,1,1,    0,0,

            1,1,1,  1,1,1,  1,1,1,1,    0,0,
            1,-1,1,  1,1,1,  1,1,1,1,    0,0,

            
            -1,1,-1,  1,1,1,  1,1,1,1,    0,0,
            1,1,-1,  1,1,1,  1,1,1,1,    0,0,

            -1,-1,-1,  1,1,1,  1,1,1,1,    0,0,
            1,-1,-1,  1,1,1,  1,1,1,1,    0,0,

            -1,1,-1,  1,1,1,  1,1,1,1,    0,0,
            -1,-1,-1,  1,1,1,  1,1,1,1,    0,0,

            1,1,-1,  1,1,1,  1,1,1,1,    0,0,
            1,-1,-1,  1,1,1,  1,1,1,1,    0,0,

            1,1,1,  1,1,1,  1,1,1,1,    0,0,
            1,1,-1,  1,1,1,  1,1,1,1,    0,0,

            -1,1,1,  1,1,1,  1,1,1,1,    0,0,
            -1,1,-1,  1,1,1,  1,1,1,1,    0,0,

            -1,-1,1,  1,1,1,  1,1,1,1,    0,0,
            -1,-1,-1,  1,1,1,  1,1,1,1,    0,0,

            1,-1,1,  1,1,1,  1,1,1,1,    0,0,
            1,-1,-1,  1,1,1,  1,1,1,1,    0,0,
        ]));

        //create models for drawing
        for(let v of fromAnim.models){
            const model=renderer.newMeshRaw(project.models[v.modelID].data);
            model.texture=MV.getTexture(project.models[v.modelID].textureID,renderer);
            anim.models.push(model)
            model.functions=v.functions;
        }
        return anim;
    }
    draw(renderer){
        this.hitBoxWireFrame.position=Vec3.add(this.hitbox.pos,this.position);
        this.hitBoxWireFrame.scale=this.hitbox.siz;
        renderer.drawLines(this.hitBoxWireFrame);
        const x=this.input.x;
        const y=this.input.y;
        const t=performance.now()/1000;
        const n=0;
        const c=1;

        const useParam=function(model,paramName){
            const expr=model.functions[paramName];
            const allowed = {
                //functions
                min:Math.min,
                max:Math.max,
                
                sin:Math.sin,
                cos:Math.cos,
                tan:Math.tan,
                
                //constants
                pi:Math.PI,
                e:Math.E,
                //my functions
                lerp:lerp,
                clamp:clamp,
                //variables
                x:x,
                y:y,
                n:n,
                c:c,
                nc:n/c,
                nm:n/(c-1),
                t:t,
            };
            const keys=Object.keys(allowed);
            const values=Object.values(allowed);
            let fn;
            try{
            fn=Function(...keys,`return(${expr})`)
            }catch(err){
                return 0;
            }
            try{
            return fn(...values);
            }catch(err){return 0;}
        }

        for(let v of this.models){
            v.position[X]=useParam(v,'x')
            v.position[Y]=useParam(v,'y')
            v.position[Z]=useParam(v,'z')

            v.rotation[X]=useParam(v,'rx')*Math.PI/180
            v.rotation[Y]=useParam(v,'ry')*Math.PI/180
            v.rotation[Z]=useParam(v,'rz')*Math.PI/180

            v.scale[X]=useParam(v,'sx')
            v.scale[Y]=useParam(v,'sy')
            v.scale[Z]=useParam(v,'sz')

            v.position=Vec3.add(v.position,this.position);

            renderer.drawMesh(v);
        }
    }
}