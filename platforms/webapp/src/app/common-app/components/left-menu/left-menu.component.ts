import { Component, Input, OnInit, Output, EventEmitter, Renderer2 } from '@angular/core';
import { TiLeftmenuItem, Util } from '@cloud/tiny3';
import { Router } from '@angular/router';

@Component({
    selector: 'app-left-menu',
    templateUrl: './left-menu.component.html',
    styleUrls: ['./left-menu.component.scss']
})
export class LeftMenuComponent implements OnInit {
    @Input() elementId: string;
    @Input() headLabel: string;
    @Input() items: TiLeftmenuItem[] = [];
    @Input() reloadState: boolean = true;  // 在单击当前路由的菜单时，是否刷新页面
    @Input() toggleable: boolean = true;   // 是否开启左侧菜单面板可折叠功能，默认开启。
    @Input() collapsed: boolean = false;   // 默认展开，当设置为true时会收起
    @Input() active: TiLeftmenuItem;

    @Output() activeChange: EventEmitter<TiLeftmenuItem> = new EventEmitter();
    @Output() collapsedChange: EventEmitter<boolean> = new EventEmitter();
    @Output() reloadStateChange: EventEmitter<boolean> = new EventEmitter();

    private stateOff: string = '0';
    private stateOn: string = '280px';
    public marginLeft: string = this.stateOn;

    constructor(
        private router: Router,
        private renderer2: Renderer2
    ) { }

    ngOnInit() {
        // 获取当前路由，选中左侧菜单的项
        this.items.forEach((item) => {
            if (this.router.url.indexOf(item.router[0]) > -1) {
                this.active = item;
            }
        });
    }

    ngAfterViewInit(): void {
        // 监听容器滚动事件，触发tiScroll收起下拉
        this.renderer2.listen(document.getElementById('navContainer'), 'scroll', () => {
            // tiScroll 是tiny3的自定义事件，可以触发面板收起
            Util.trigger(document, 'tiScroll');
        });
    }

    clickLevel1(ev, activeItem: TiLeftmenuItem) {
        // 选择菜单项时，更改选中的菜单项
        this.active = activeItem;
        // 解决路由跳转时，组件多执行一次生命周期的问题
        ev.preventDefault();
        ev.stopPropagation();
    }

    changeActive() {
        // 发送菜单选中项变更事件
        this.activeChange.emit(this.active);
    }
    changeCollapas() {
        // 发送左侧菜单折叠状态
        this.collapsedChange.emit(this.collapsed);
    }
    changeReload() {
        // 右侧内容是否需要刷新的状态改变值
        this.reloadStateChange.emit(this.reloadState);
    }

    public toggleClick(isHide: boolean): void {
        // 需要业务侧在菜单收起、展开时，控制右侧内容的位置
        this.marginLeft = isHide ? this.stateOff : this.stateOn;
    }

}
