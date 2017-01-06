var W3CDOM = (document.createElement && document.getElementsByTagName);

window.onload = initialize;

function initialize() {
    if (!W3CDOM)
        return;

    var fakeFileUpload = document.createElement('div');
    fakeFileUpload.className = 'file-overlay';
    fakeFileUpload.appendChild(document.createElement('input'));
    var image = document.createElement('img');
    image.src = '/file-select.gif';
    fakeFileUpload.appendChild(image);
    var x = document.getElementsByTagName('input');

    for (var i = 0; i != x.length; i++) {
        if (x[i].type != 'file')
            continue;
        if (x[i].parentNode.className != 'file-input')
            continue;

        x[i].className = 'file';
        var clone = fakeFileUpload.cloneNode(true);
        x[i].parentNode.appendChild(clone);
        x[i].relatedElement = clone.getElementsByTagName('input')[0];

        x[i].onchange = x[i].onmouseout = function () {
            this.relatedElement.value = this.value;
        }
    }
}
