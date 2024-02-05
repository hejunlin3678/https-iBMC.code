import { Pipe, PipeTransform } from '@angular/core';
import { interNameArr, classNameArr } from '../models/common.interface';

@Pipe({
    name: 'processObject'
})
export class ProcessObjectPipe implements PipeTransform {
    /**
     * 当前仅支持number、string、boolean和元祖数据类型转换成键值对，对象和数组不进行转换
     * @param obj 转换对象
     * @param args
     */
    transform(obj: {}, args?: any): object[] {
        const arr = [];
        // tslint:disable-next-line: forin
        for (const key in obj) {
            const isTuple = this.isTuple(obj[key]);
            if ((obj[key] === 0 || obj[key]) && (this.checkType(obj[key]) || (isTuple && (obj[key][1] === 0 || obj[key][1])))) {
                arr.push({
                    key,
                    value: this.isOption(obj[key], isTuple),
                    className: this.getClassName(obj[key][1]),
                    isTuple
                });
            }
        }
        return arr;
    }

    /**
     * 判定对象是否是元祖类型
     * @param param 判定对象
     */
    isTuple(param: any): boolean {
        if (param instanceof Array) {
            // 元祖第一个元素作为国际化key, 如果非string，国际化翻译报错
            if (typeof param[0] !== 'string') {
                return false;
            }
            let index = 0;
            param.forEach((val) => {
                if (typeof val === 'number' || typeof val === 'string' || typeof val === 'object') {
                    index++;
                } else {
                    return false;
                }
            });
            if (index === param.length) {
                return true;
            }
        }
        return false;
    }

    isOption(param: any, isTuple: boolean): string {
        const par = isTuple ? param[1] : param;
        const key = Object.prototype.toString.call(par);
        let returnStr = null;
        switch (key) {
            case '[object String]':
                returnStr = par;
                break;
            case '[object Object]':
                if (par.name && interNameArr.indexOf(par.name) > -1) {
                    returnStr = par.label;
                }
                break;
            default:
                returnStr = String(par);
                break;
        }
        returnStr = isTuple ? [param[0], returnStr] : returnStr;
        return returnStr;
    }

    getClassName(param: any): string {
        let returnStr = null;
        if (Object.prototype.toString.call(param) === '[object Object]') {
            returnStr =  classNameArr.indexOf(param.name) > -1 ? param.className : null;
        }
        return returnStr;
    }

    /**
     * 判定是否是number、string、boolean数据类型
     * @param param 判定对象
     */
    checkType(param: any): boolean {
        if (typeof param === 'number' || typeof param === 'string' || typeof param === 'boolean') {
            return true;
        }
        return false;
    }
}
