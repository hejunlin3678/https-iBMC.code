import { Component, OnInit, Input } from '@angular/core';
import { TiButtonItem } from '@cloud/tiny3';
import { LanguageService } from 'src/app/common-app/service';
import { PowerCapping } from '../../model/power-capping';
import { PowerCappingHistory } from '../../model/power-capping-history';

@Component({
    selector: 'app-power-capping',
    templateUrl: './power-capping.component.html',
    styleUrls: ['./power-capping.component.scss']
})
export class PowerCappingComponent implements OnInit {

    @Input() powerCapping: PowerCapping;
    @Input() powerHistory: PowerCappingHistory;
    @Input() isPrivileges: boolean;

    constructor(private lang: LanguageService) { }

    public timeList: string[];
    public powerPeakWattsList:  object[];
    public powerAverageWattsList: object[];

    public powerUnit: string = 'W';

    public powerUnits: TiButtonItem[] = [ {
        id: 'BTU',
        text: 'BTU/h'
    }, {
        id: 'W',
        text: 'W'
    }];
    public selected: TiButtonItem =  this.powerUnits[0];
    public powerUnitsChange() {
        this.powerUnit = this.selected.id;
    }

    init() {
        this.timeList = this.powerHistory.getTimeList().slice(0);
        this.powerPeakWattsList = this.powerHistory.getPowerPeakWattsList().slice(0);
        this.powerAverageWattsList = this.powerHistory.getPowerAverageWattsList().slice(0);
    }

    ngOnInit(): void {
        this.selected = this.lang.locale === 'zh-CN' ? this.powerUnits[1] : this.powerUnits[0];
        this.powerUnit = this.selected.id;
        this.init();
    }

}
