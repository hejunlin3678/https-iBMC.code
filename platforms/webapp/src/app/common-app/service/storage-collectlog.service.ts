import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';

@Injectable({
    providedIn: 'root'
})
export class StorageCollectLogService {
    constructor() {
        this.changeState.subscribe((res: { key: string; value: boolean }) => {
            const { key, value } = res;
            this.collectedState[key] = value;
        });
    }
    public collectedState: any = {};
    public changeState: Subject<{ key: string; value: boolean }> = new Subject();

    // 获取当前的日志收集状态
    public getCollectedState(key: string): boolean {
        return this.collectedState[key];
    }
}
