import { Component, OnChanges, Input, SimpleChanges } from '@angular/core';

@Component({
    selector: 'app-second-drop',
    templateUrl: './second-drop.component.html',
    styleUrls: ['./second-drop.component.scss']
})
export class SecondDropComponent implements OnChanges {
    menuState: string = 'none';
    @Input() state: string = 'leave';
    @Input() id: string = '';

    constructor() { }

    ngOnChanges(changes: SimpleChanges): void {
        this.menuState = changes.state.currentValue === 'enter' ? 'block' : 'none';
    }

    public showDropMenu() {
        this.menuState = 'block';
    }

    public hideDropMenu(ev) {
        const position = ev.target.getBoundingClientRect();
        const height = ev.target.offsetHeight;
        const bottomY = position.y + height;
        // 如果鼠标的位置超出元素的底边框位置，则不隐藏下拉框
        if (ev.clientY < bottomY) {
            this.menuState = 'none';
        }
    }

}
