import { isLowBrowserVer } from '../utils';

export class GlobalData {

    private static instance: GlobalData;
    private softwareName: string;
    private smsName: string;
    private webStyle: string;
    private isLowBrowserVer: boolean;
    private isShenmaStyle: boolean;

    private constructor() { }

    public static getInstance(): GlobalData {
        if (!GlobalData.instance) {
            GlobalData.instance = new GlobalData();
        }
        return GlobalData.instance;
    }

    set setWebStyle(webStyle: string) {
        this.webStyle = webStyle;
    }

    get getWebStyle() {
        return this.webStyle;
    }

    set setShenmaStyle(isShenmaStyle: boolean) {
        this.isShenmaStyle  =  isShenmaStyle;
    }

    get getShenmaStyle() {
        return this.isShenmaStyle;
    }

    set setSoftwareName(softwareName: string) {
        this.softwareName = softwareName;
    }

    get getSoftwareName() {
        return this.softwareName;
    }

    set setSmsName(smsName: string) {
        this.smsName = smsName;
    }

    get getSmsName() {
        return this.smsName;
    }

    get getIsLowBrowserVer() {
        if (this.isLowBrowserVer === null || this.isLowBrowserVer === undefined) {
            this.isLowBrowserVer = isLowBrowserVer();
        }
        return this.isLowBrowserVer;
    }
}
