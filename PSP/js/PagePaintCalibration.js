function selectDirectory()
{
    return logicPagePaintCalibration.selectDirectory();
}

function getFileBaseName(pathName)
{
    return logicPagePaintCalibration.getFileBaseName(pathName);
}

function getModelImages()
{
    return JSON.parse(logicPagePaintCalibration.getModelImage())["model"];
}

function getModelParameter()
{
    return JSON.parse(logicPagePaintCalibration.getModelParameter())["model"];
}

function moveUpIndexItem(index, model)
{
    if(index <= 0 || index > model.length - 1) return false;

    var upperItem = model[index - 1];
    model[index - 1] = model[index];
    model[index] = upperItem;

    return new Array(index - 1, model);
}

function moveDownIndexItem(index, model)
{
    if(index < 0 || index >= model.length - 1) return false;

    var downItem = model[index + 1];
    model[index + 1] = model[index];
    model[index] = downItem;

    return new Array(index + 1, model);
}

function setAsBackground(index, model)
{
    if(index < 0 || index >= model.length) return false;

    for(var i = 0 ; i < model.length; i = i+1)
    {
        model[i].isBackground = false;

        if(i === index)
        {
            model[i].isBackground = true;
            model[i].isReference = false;
            model[i].P = "";
            model[i].T = "";
        }
    }

    var item = model.splice(index, 1);
    model.splice(0,0, item[0]);

    index = 0;
    return new Array(index, model);
}

function setAsReference(index, model)
{
    if(index < 0 || index > model.length - 1) return false;

    for(var i = 0; i < model.length - 1; i = i + 1)
    {
        model[i].isReference = false;

        if(i === index)
        {
            model[i].isReference = true;
            model[i].isBackground = false;
        }
    }

    var item = model.splice(index,1);
    if(model[0].isBackground === true)
    {
        model.splice(1, 0, item[0]);
        index = 1;
    }
    else
    {
        model.splice(0, 0, item[0]);
        index = 0;
    }

    return new Array(index, model);
}

function applyFit(modelImages, modelParameters)
{
    if(modelImages.length <=2 || modelParameters.length < 1) return false;

    if(modelImages[0].isBackground !== true || modelImages[1].isReference !== true) return false;

    for(var i = 0; i < Math.min(modelImages.length - 1, modelParameters.length); i = i + 1)
    {
        modelImages[i + 1].P = modelParameters[i].P;
        modelImages[i + 1].T = modelParameters[i].T;
    }

    return new Array(modelImages, modelParameters);
}

function applyModelImages(model)
{
    var modelImage = new Object();
    modelImage["model"] = model;
    logicPagePaintCalibration.setModelImage(JSON.stringify(modelImage));
}
