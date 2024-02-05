export class BrowserType {
    constructor() { }

    getBrowserType() {
        let browser = '';
        let version = 0;
        const userAgent = navigator.userAgent;
        const isOpera = userAgent.indexOf('Opera') > -1;
        const isIE = userAgent.indexOf('Trident') > -1
            || (userAgent.indexOf('compatible') > -1 && userAgent.indexOf('MSIE') > -1)
            && !isOpera;
        const isEdge = userAgent.indexOf('Edg') > -1 && !isIE;
        const isIE11 = userAgent.indexOf('rv:11.0') > -1;
        const isFirefox = userAgent.indexOf('Firefox') !== -1;
        const isChrome = userAgent.indexOf('Chrome') && window['chrome'];
        if (isIE || isIE11) {
            browser = 'IE';
        } else if (isFirefox) {
            browser = 'Firefox';
            const ff = userAgent.match(/Firefox\/([\d.]+)/);
            if (ff) {
                version = Number(ff[1]);
            }
        } else if (isChrome) {
            browser = 'Chrome';
            const cc = userAgent.match(/Chrome\/([\d.]+)/);
            if (cc) {
                version = Number(cc[1].split('.')[0]);
            }
        } else if (isEdge) {
            browser = 'isEdge';
        } else {
            browser = 'Other';
        }
        return {
            browser,
            version
        };
    }
}
