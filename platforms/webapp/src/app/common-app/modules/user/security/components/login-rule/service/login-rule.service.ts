import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { TiTableRowData } from '@cloud/tiny3';
import { ITableRow } from '../login-rule.datatype';
import { HttpService } from 'src/app/common-app/service';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';

@Injectable({
    providedIn: 'root'
})
export class LoginRuleService {

    private readonly RULE_NAME = ['SECURITY_SECUTITY_CFG_RULE1', 'SECURITY_SECUTITY_CFG_RULE2', 'SECURITY_SECUTITY_CFG_RULE3'];

    public getData(): Observable<TiTableRowData[]> {
        return this.getLoginRule().pipe(
            pluck('body'),
            map(data => this.ruleInit(data))
        );
    }

    private getLoginRule() {
        const url = '/UI/Rest/AccessMgnt/LoginRule';
        return this.http.get(url);
    }

    private updateLoginRule(param) {
        const url = '/UI/Rest/AccessMgnt/LoginRule';
        return this.http.patch(url, param).pipe(
            pluck('body')
        );
    }

    private ruleInit(data): TiTableRowData[] {
        const {
            Members: members
        } = data;

        const tableData: TiTableRowData[] = [];
        if (Array.isArray(members)) {
            members.forEach((item) => {
                const transformedItem: TiTableRowData = {
                    memberId: item.ID,
                    ruleName: this.RULE_NAME[item.ID],
                    startTime: item.StartTime || '',
                    endTime: item.EndTime || '',
                    isSave: false,
                    isEdit: false,
                    IP: item.IP,
                    mac: item.Mac,
                    ruleEnabled: item.Status,
                    offLabel: 'OFF',
                    onLabel: 'ON',
                    disable: false
                };
                tableData.push(transformedItem);
            });
        }

        return tableData;
    }

    public saveRule(row: ITableRow) {
        const param = {
            ID: row.memberId,
            Status: row.ruleEnabled,
            IP: row.ipValidation.value || '',
            Mac: row.macValidation.value || '',
            StartTime: row.timeForm.controls.startTime.value || '',
            EndTime: row.timeForm.controls.endTime.value || '',
        };

        return this.updateLoginRule(param);
    }

    constructor(private http: HttpService) { }

}
