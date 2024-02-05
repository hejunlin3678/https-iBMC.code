import { Injectable, EventEmitter } from '@angular/core';
import { Subject } from 'rxjs';

@Injectable({
    providedIn: 'root'
})

export class IrmGlobalDataService {
    constructor() { }
    // 首页跳转资产管理选中的id
    public assetUid: number = -1;
}
