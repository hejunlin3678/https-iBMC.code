import { Injectable } from '@angular/core';
import { AlertMessageService } from './alertMessage.service';
import * as utils from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';

@Injectable({
    providedIn: 'root'
})
export class ErrortipService {

    constructor(
        private alertService: AlertMessageService,
        private translate: TranslateService
    ) {

    }

    // 获取错误ID 对应的国际化文字
    public getErrorMessage(errorId) {
        let result = '';
        const errorMessage = this.translate.instant(errorId);
        if (errorMessage && typeof errorMessage === 'object') {
            result = errorMessage['attrMessage'] || errorMessage['errorMessage'];
        }
        return result;
    }

    // 错误处理方法
    public errorTip(error) {
        // 401错误已经在httpService中过滤掉了，该服务主要处理的是400和200的错误处理
        const errorId = utils.getMessageId(error)[0].errorId;
        // 如果是授权失败错误，则不提示
        if (errorId === 'AuthorizationFailed') {
            return;
        }

        if (errorId) {
            // 使用右上角的错误提示
            const errorMessage = this.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alertService.eventEmit.emit({
                type: 'error',
                label: errorMessage
            });
        }
    }

    /**
     * 获取错误对象的ID，参数举值如下
     * 1. 响应码为200，包含错误对象的响应
     * 2. 响应码为4xx，包含错误对象的响应
     * 3. 默认返回第一个错误ID，如果有需要返回所有的ID，则需要传入第二个参数为true
     * 4. 如果getAll为false, 则默认返回第一个错误ID，否则返回错误ID数据
     */
    public getAllErrorId(response, getAll: boolean = false): string | string[] {
        return '';
    }
}
