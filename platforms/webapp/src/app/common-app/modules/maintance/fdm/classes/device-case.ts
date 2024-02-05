import { CaseItem } from './caseItem';
import { TranslateService } from '@ngx-translate/core';
import { TiPageSizeConfig, TiDateValue } from '@cloud/tiny3';

export class DeviceCase {

    public titles: IThs[];
    public typeArr: IOption[];
    public typeSelected: IOption;
    public levelArr: IOption[];
    public levelSelected;
    private caseArr: CaseItem[];
    public displayed: CaseItem[];
    public totalCount: number;
    public currentPage: number;
    public pageSize: TiPageSizeConfig;
    public srcData: any;
    public onNgModelChange;
    public dateValue: TiDateValue;
    public dateFormat: string;
    public dateAlign: string;

    constructor(
        private i18n: TranslateService,
        caseArr: CaseItem[]
    ) {
        this.titles = [
            {
                title: 'FDM_ALARM_NUM',
                width: '8%'
            }, {
                title: 'FDM_ALARM_TYPE',
                width: '15%'
            }, {
                title: 'FDM_ALARM_LEVEL',
                width: '15%'
            }, {
                title: 'FDM_ALARM_CREATED',
                width: '17%'
            }, {
                title: 'FDM_INFO',
                width: '45%'
            }
        ];
        this.typeArr = [
            {
                id: 'All',
                label: this.i18n.instant('FDM_ALARM_ALL_TYPE')
            }, {
                id: 'FDM Warning',
                label: this.i18n.instant('FDM_WARNING')
            }, {
                id: 'SEL Warning',
                label: this.i18n.instant('FDM_SEL_WARNING')
            }, {
                id: 'Installation',
                label: this.i18n.instant('FDM_INSTALLATION')
            }
        ];
        this.typeSelected = this.typeArr[0];
        this.levelArr = [
            {
                id: 'All',
                label: this.i18n.instant('FDM_ALARM_ALL_LEVEL')
            }, {
                id: 'Critical',
                label: this.i18n.instant('FDM_ALARM_CRITICAL_SHORT')
            }, {
                id: 'Major',
                label: this.i18n.instant('ALARM_EVENT_MAJOR')
            }, {
                id: 'Minor',
                label: this.i18n.instant('FDM_MINOR_ALARM_SHORT')
            }, {
                id: 'Event',
                label: this.i18n.instant('FDM_TIME_LOG_SHORT')
            }
        ];
        this.levelSelected = this.levelArr[0];
        this.dateFormat = 'yyyy/MM/dd';
        this.dateAlign = 'right';
        this.caseArr = caseArr;
        this.displayed = [];
        this.srcData = {
            data: this.caseArr,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        this.pageSize = {
            options: [5, 10, 20, 40, 60],
            size: 5
        };
        this.currentPage = 1;
        this.totalCount = this.srcData.data.length;
    }

    public setType(selected: IOption) {
        this.typeSelected = selected;
        this.filtrateData();
        this.resetPage();
    }
    public setLevel(selected: IOption) {
        this.levelSelected = selected;
        this.filtrateData();
        this.resetPage();
    }
    public setTime(value: TiDateValue) {
        this.dateValue = value;
        this.filtrateData();
        this.resetPage();
    }
    public filtrateData() {
        const temp = [];
        let begin;
        let end;
        if (this.dateValue) {
            begin = this.dateValue.begin.getTime();
            end = this.dateValue.end.getTime();
        }
        this.caseArr.forEach((item: CaseItem) => {
            if (!item) { return; }
            const now = new Date(item.getCreateTime.replace(/-/g, '/')).getTime();
            if ((this.typeSelected.id === 'All' || item.getType === this.typeSelected.id)
                && (this.levelSelected.id === 'All' || item.getLevel === this.levelSelected.id)
                && (this.dateValue === null || (begin <= now && end >= now))) {
                temp.push(item);
            }
        });
        this.srcData.data = temp;
    }
    public resetPage() {
        this.currentPage = 1;
        this.totalCount = this.srcData.data.length;
    }
}

interface IThs {
    title: string;
    width: string;
}

interface IOption {
    id: string;
    label: string;
}
