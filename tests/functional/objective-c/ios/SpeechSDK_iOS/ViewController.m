//
//  ViewController.m
//  SpeechSDK_iOS
//
//  Created by carbon on 11.08.18.
//  Copyright © 2018 MSFT. All rights reserved.
//

#import "ViewController.h"
#import "../../shared/test.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [EndToEndTests runTest];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
