import { Directive, ElementRef, Input, Output, EventEmitter, Attribute } from '@angular/core';

@Directive({
    selector: '[reFire]'
})
export class RefireDirective {
    @Input() ctrlName: string = '';
    @Output() tagChange: EventEmitter<any> = new EventEmitter();

    private element: HTMLInputElement;
    constructor(
        private eleRef: ElementRef,
        @Attribute('formControlName') private controlName: string
    ) {
        this.element = this.eleRef.nativeElement;
    }

    ngAfterViewInit(): void {
        this.element.addEventListener('change', () => {
            const value = this.element.value;
            const controlName = this.controlName || this.ctrlName || this.element.name;
            const param = { name: controlName, value };
            if (value === '' || value === null) {
                this.tagChange.emit(param);
            }
        });
    }
}
