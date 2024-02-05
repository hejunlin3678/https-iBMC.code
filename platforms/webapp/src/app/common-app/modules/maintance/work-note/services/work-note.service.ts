import { HttpService } from 'src/app/common-app/service';
import { Injectable } from '@angular/core';
import { IAddRecord, IRecord, IEditRecord } from '../work-note.datatype';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
/**
 * 如果采用 `providedIn` ，
 * 这个形式是 Angular v6 之后引入的
 * 这种写法和传统的在 Module 中设置 providers 数组的写法的区别在于
 * 可以让服务在真正被其它组件或服务注入时才编译到最后的 js 中
 * 对于引入第三方类库较多的应用可以有效减小 js 大小
 */
@Injectable({
    providedIn: 'root'
})
export class WorkNoteService {

    constructor(private http: HttpService) {
    }

    public getData() {
        return this.getRecord();
    }

    public getIp() {
        return this.getSession();
    }

    private getSession(): Observable<string> {
        const url = '/UI/Rest/Sessions';
        return this.http.get(url).pipe(
            pluck('body', 'Members'),
            map(members => this.factoryIp(members))
        );
    }

    private factoryIp(members): string {
        const selfMember = members.find(member => member.MySession);
        return selfMember.IPAddress;
    }

    private getRecord(): Observable<IRecord[]> {
        const url = '/UI/Rest/Maintenance/WorkRecord';
        return this.http.get(url).pipe(
            pluck('body'),
            map((data) => this.factoryRecords(data))
        );
    }

    public addRecord(details: IAddRecord) {
        const url = '/UI/Rest/Maintenance/WorkRecord';
        return this.http.post(url, details);
    }

    public updateRecord(details: IEditRecord) {
        const url = '/UI/Rest/Maintenance/WorkRecord';
        return this.http.put(url, details);
    }

    public deleteRecord(id: string) {
        const url = `/UI/Rest/Maintenance/WorkRecord/${id}`;
        return this.http.delete(url).pipe(
            pluck('body')
        );
    }


    private factoryRecords(res): IRecord[] {
        const records: any[] = res.List;
        if (!records || !records.length) {
            return [];
        }

        const transformedRecords: IRecord[] = records.map((record) => {
            return {
                id: record.ID,
                user: record.UserName,
                ip: record.IPAddress,
                time: record.Time,
                details: record.Content,
            };
        });

        return transformedRecords.reverse();

    }
}
