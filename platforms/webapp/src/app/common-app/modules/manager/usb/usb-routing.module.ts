import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { USBComponent } from './usb.component';

const routes: Routes = [
    {
        path: '',
        component: USBComponent
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class UsbRoutingModule { }
