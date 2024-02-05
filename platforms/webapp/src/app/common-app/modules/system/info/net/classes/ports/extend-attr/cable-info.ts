import {OpticalAttr} from './optical-attr';

export class CableInfo {
    public attr = [];
    constructor(
        title1: string | OpticalAttr,
        optInfo: OpticalAttr,
        name: string
    ) {
        this.attr.push({
            title: title1,
            name,
            info: optInfo
        });
    }
}
