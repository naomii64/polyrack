GLOBAL.init();
//ui
TabMenus.init();
UIHelper.init();
//importing files
Importer.init();
//menus
LayoutEditor.init();
TextureEditor.init();
ModelViewer.init();
AnimationEditor.init();
FileMenu.init();
//setup drawing so that only one tab is running rendering stuff at once
TabMenus.setTabDrawFunction("main-tab-menu","layout-tab",LayoutEditor.animate);
TabMenus.setTabDrawFunction("main-tab-menu","models-tab",ModelViewer.animate);
TabMenus.setTabDrawFunction("main-tab-menu","animation-tab",AnimationEditor.animate);