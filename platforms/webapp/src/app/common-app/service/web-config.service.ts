import { Injectable } from '@angular/core';

@Injectable({
    providedIn: 'root'
})
export class WebConfigService {
    // 需要替换的属性名
    public REPLACE_ORIGIN: string = 'Huawei';
    public REPLACE_TARGET: string = 'mainyfest';

    constructor() { }

    // 该方法将可根据传入参数 direct，来决定是将 Huawei 替换为 manyfest还是将 mainyfest替换为Huawei  direct: 取值[1,-1]，1代表Huawei替换为mainyfest，-1则相反
    public replaceContent(content: any, direct: number) {
        // 如果是基础数据类型，则直接退出
        if (typeof content !== 'object') {
            return;
        } else {
            const type = Object.prototype.toString.call(content);
            if (type === '[object Object]') {
                for (const key in content) {
                    if (Object.prototype.hasOwnProperty.call(content, key)) {
                        const keyOrigin = direct === 1 ? this.REPLACE_ORIGIN : this.REPLACE_TARGET;
                        const keyTarget = direct === 1 ? this.REPLACE_TARGET : this.REPLACE_ORIGIN;
                        if (key === keyOrigin) {
                            const value = content[key];
                            content[keyTarget] = value;
                            delete content[key];
                            this.replaceContent(content[keyTarget], direct);
                        }
                        this.replaceContent(content[key], direct);
                    }
                }
            }
            if (type === '[object Array]') {
                for (const item of content) {
                    this.replaceContent(item, direct);
                }
            }
        }
        return content;
    }
}
