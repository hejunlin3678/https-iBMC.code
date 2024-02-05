import { Component, OnInit, Input, Output, EventEmitter, SimpleChanges } from '@angular/core';
import { TiButtonItem, TiDateValue, TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { IFilterSelect } from '../../domain';

@Component({
    selector: 'app-filter-condition',
    templateUrl: './filter-condition.component.html',
    styleUrls: ['./filter-condition.component.scss']
})
export class FilterConditionComponent implements OnInit {
    constructor(private translate: TranslateService, private tiMessage: TiMessageService) { }
    public clearLogTip = this.translate.instant('COMMON_CLEAR');
    public downloadLogTip = this.translate.instant('ALARM_EVENT_DOWNLOAD');
    public collapsed: boolean = true;
    @Input() conditionsTitle: TiButtonItem[] = [];
    @Input() searchValue: string = '';
    @Input() searchPlacehoder: string = '';
    @Input() format1: string = '';
    @Input() value1: TiDateValue;
    @Input() downloading;
    @Input() isSystemLock;
    @Input() privileges;
    @Output() downLog = new EventEmitter<string>();
    @Output() queryLog = new EventEmitter<object>();
    @Output() reset = new EventEmitter();
    @Output() clearLogEvent = new EventEmitter();

    // 筛选条件回显Model
    public filterSelected: IFilterSelect[] = [{
        id: 'AlarmEventSelect',
        text: '',
        show: false,
        uniq: null
    }, {
        id: 'subjectTypeSelect',
        text: '',
        show: false,
        uniq: null
    }, {
        id: 'timeSelect',
        text: '',
        show: false,
        uniq: null
    }];
    ngOnInit() {
    }
    // 清除输入框内容
    public clear(): void {
        this.searchValue = '';
    }
    ngOnChanges(changes: SimpleChanges): void {
    }
    // 初始化，隐藏已选中搜索条件
    public initData() {
        this.filterSelected.forEach(item => item.show = false);
    }
    // 清空日志
    public clearLog() {
        if (this.isSystemLock) {
            return;
        }
        const _self = this;
        const instance = this.tiMessage.open({
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click(): void {
                    instance.close();
                    _self.clearLogEvent.emit();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click(): void {
                    instance.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }
    // 下载日志
    public downloadLog() {
        const fileName = Date.now() + '_sel.tar';
        this.downLog.emit(fileName);
    }
    // 查询
    public query() {
        this.collapsed = true;
        let levelIds = [];
        if (this.conditionsTitle[0].selected?.length > 0) {
            this.conditionsTitle[0].selected.forEach(element => {
                levelIds.push(element.id);
            });
        }
        if (levelIds.length === 1 && levelIds[0] === null) {
            levelIds = [];
        }
        // 设置查询参数（Subject：主题类型）
        let subjectIds = [];
        if (this.conditionsTitle[1].selected?.length > 0) {
            this.conditionsTitle[1].selected.forEach(element => {
                subjectIds.push(element.id);
            });
        }
        if (subjectIds.length === 1 && subjectIds[0] === null) {
            subjectIds = [];
        }
        const params = {
            Level: levelIds,
            Subject: subjectIds,
            SearchString: this.searchValue,
            value1: this.value1,
            timeSelect: this.conditionsTitle[2].selected?.id
        };
        this.conditionsTitle.forEach((item, index) => {
            if (
              (item.selected &&
                item.selected.length > 0 &&
                item.selected[0].id !== null) ||
              (item.selected?.id && item.selected.id !== null)
            ) {
              // 筛选条件关键字回显逻辑调整(多选以|分隔回显)
              if (item.id !== 3) {
                const filterTextContent = [];
                const filterIds = [];
                item.selected.forEach((element) => {
                  filterTextContent.push(element.text);
                  filterIds.push(element.id);
                });
                this.filterSelected[index].text = filterTextContent.join('|');
                this.filterSelected[index].uniq = filterIds;
              } else {
                this.filterSelected[index].text = item.selected.text;
                this.filterSelected[index].uniq = item.selected?.id;
              }
              this.filterSelected[index].show = true;
            } else {
              this.filterSelected[index].show = false;
            }
        });
        this.queryLog.emit(params);
    }
    // 重置
    public resetSearch() {
        this.reset.emit();
        this.searchValue = '';
    }
    // 清除选择
    public clearSelect(id) {
        this.collapsed = true;
        switch (id) {
            case 'AlarmEventSelect':
                this.filterSelected[0].show = false;
                this.filterSelected[0].text = this.translate.instant('ALARM_EVENT_ALL');
                this.filterSelected[0].uniq = [null];
                // 告警级别选中（多选）：全部
                this.conditionsTitle[0].selected = [this.conditionsTitle[0].items[0]];
                break;
            case 'subjectTypeSelect':
                this.filterSelected[1].show = false;
                this.filterSelected[1].text = this.translate.instant('ALARM_EVENT_ALL');
                this.filterSelected[1].uniq = [null];
                // 主题类型选中（多选）：全部
                this.conditionsTitle[1].selected = [this.conditionsTitle[1].items[1]];
                break;
            case 'timeSelect':
                this.filterSelected[2].show = false;
                this.filterSelected[2].text = this.translate.instant('ALARM_EVENT_ALL');
                this.filterSelected[2].uniq = null;
                break;
            default:
        }
        const params = {
            Level: this.filterSelected[0].uniq,
            Subject: this.filterSelected[1].uniq,
            SearchString: this.searchValue,
            value1: this.value1,
            timeSelect: this.filterSelected[2].uniq,
        };
        this.queryLog.emit(params);
    }

    // modelChange事件校验全选与其它项多选冲突
    modelChange(conditionConfig): void {
        // 存在多选，冲突处理
        if (conditionConfig.selected && conditionConfig.selected.length > 1) {
            this.conditionsTitle.forEach(condition => {
                if (condition.id === conditionConfig.id) {
                    // 最新选中的选项逻辑判断（全部选项与其它选项单选、多选存在互斥逻辑）
                    if (conditionConfig.selected[conditionConfig.selected.length - 1].id === null) {
                        // 最近新添加的选项是“全部”，清空其它项
                        condition.selected = [condition.items[0]];
                    } else {
                        // 最近新添加的选项不是“全部”，已选中“全部”选项则立即删除“全部”选项
                        const isAllOption = (element): boolean => element.id === null;
                        const indexAll = condition.selected.findIndex(isAllOption);
                        if (indexAll !== -1) {
                            // 立即删除“全部”选项独立逻辑
                            condition.selected.splice(indexAll, 1);
                        }
                    }
                }
            });
        }
    }
}
