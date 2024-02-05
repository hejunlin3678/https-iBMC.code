import { ThresholdAttr } from './threshold-attr';
import { OpticalAttr } from './optical-attr';

export class OpticalInfo {
    public attr = [];
    constructor(
        title1: string | OpticalAttr,
        optInfo: OpticalAttr,
        thresInfo: ThresholdAttr,
        name: string,
    ) {
        this.attr.push({
            title: title1,
            name,
            info: optInfo
        });
        if (thresInfo) {
            this.attr.push({
                title: 'THRESHOLD_INFO',
                info: thresInfo
            });
        }
    }
}
