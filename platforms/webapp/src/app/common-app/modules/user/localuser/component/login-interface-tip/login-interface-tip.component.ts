import { Component, Input, OnInit } from '@angular/core';
@Component({
  selector: 'app-login-interface-tip',
  templateUrl: './login-interface-tip.component.html',
  styleUrls: ['./login-interface-tip.component.scss'],
})
export class LoginInterfaceTipComponent implements OnInit {
  @Input() msgTips: [];
  constructor() { }

  ngOnInit(): void {
  }

  public close(): void {

  }

  public dismiss(): void {

  }
}
