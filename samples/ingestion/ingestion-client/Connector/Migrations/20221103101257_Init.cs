// <copyright file="20221103101257_Init.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

#nullable disable

namespace Connector.Migrations
{
    using System;
    using Microsoft.EntityFrameworkCore.Migrations;

    public partial class Init : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            _ = migrationBuilder ?? throw new ArgumentNullException(nameof(migrationBuilder));

            migrationBuilder.CreateTable(
                name: "Transcriptions",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Locale = table.Column<string>(type: "nvarchar(255)", maxLength: 255, nullable: true),
                    Name = table.Column<string>(type: "nvarchar(500)", maxLength: 500, nullable: true),
                    Source = table.Column<string>(type: "nvarchar(500)", maxLength: 500, nullable: true),
                    Timestamp = table.Column<DateTime>(type: "datetime2", nullable: false),
                    Duration = table.Column<string>(type: "nvarchar(26)", maxLength: 26, nullable: true),
                    DurationInSeconds = table.Column<float>(type: "real", nullable: false),
                    NumberOfChannels = table.Column<int>(type: "int", nullable: false),
                    ApproximateCost = table.Column<float>(type: "real", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Transcriptions", x => x.Id);
                });

            migrationBuilder.CreateTable(
                name: "CombinedRecognizedPhrases",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Channel = table.Column<int>(type: "int", nullable: false),
                    Lexical = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Itn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    MaskedItn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Display = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    SentimentNegative = table.Column<float>(type: "real", nullable: false),
                    SentimentNeutral = table.Column<float>(type: "real", nullable: false),
                    SentimentPositive = table.Column<float>(type: "real", nullable: false),
                    TranscriptionsId = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_CombinedRecognizedPhrases", x => x.Id);
                    table.ForeignKey(
                        name: "FK_CombinedRecognizedPhrases_Transcriptions_TranscriptionsId",
                        column: x => x.TranscriptionsId,
                        principalTable: "Transcriptions",
                        principalColumn: "Id");
                });

            migrationBuilder.CreateTable(
                name: "RecognizedPhrases",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    RecognitionStatus = table.Column<string>(type: "nvarchar(32)", maxLength: 32, nullable: true),
                    Speaker = table.Column<int>(type: "int", nullable: false),
                    Channel = table.Column<int>(type: "int", nullable: false),
                    Offset = table.Column<string>(type: "nvarchar(26)", maxLength: 26, nullable: true),
                    Duration = table.Column<string>(type: "nvarchar(26)", maxLength: 26, nullable: true),
                    CombinedRecognizedPhrasesId = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_RecognizedPhrases", x => x.Id);
                    table.ForeignKey(
                        name: "FK_RecognizedPhrases_CombinedRecognizedPhrases_CombinedRecognizedPhrasesId",
                        column: x => x.CombinedRecognizedPhrasesId,
                        principalTable: "CombinedRecognizedPhrases",
                        principalColumn: "Id");
                });

            migrationBuilder.CreateTable(
                name: "NBests",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Confidence = table.Column<float>(type: "real", nullable: false),
                    Lexical = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Itn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    MaskedItn = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    Display = table.Column<string>(type: "nvarchar(max)", nullable: true),
                    SentimentNegative = table.Column<float>(type: "real", nullable: false),
                    SentimentNeutral = table.Column<float>(type: "real", nullable: false),
                    SentimentPositive = table.Column<float>(type: "real", nullable: false),
                    RecognizedPhrasesId = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_NBests", x => x.Id);
                    table.ForeignKey(
                        name: "FK_NBests_RecognizedPhrases_RecognizedPhrasesId",
                        column: x => x.RecognizedPhrasesId,
                        principalTable: "RecognizedPhrases",
                        principalColumn: "Id");
                });

            migrationBuilder.CreateTable(
                name: "Words",
                columns: table => new
                {
                    Id = table.Column<Guid>(type: "uniqueidentifier", nullable: false),
                    Word = table.Column<string>(type: "nvarchar(500)", maxLength: 500, nullable: true),
                    Offset = table.Column<string>(type: "nvarchar(26)", maxLength: 26, nullable: true),
                    Duration = table.Column<string>(type: "nvarchar(26)", maxLength: 26, nullable: true),
                    Confidence = table.Column<float>(type: "real", nullable: false),
                    NBestsId = table.Column<Guid>(type: "uniqueidentifier", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Words", x => x.Id);
                    table.ForeignKey(
                        name: "FK_Words_NBests_NBestsId",
                        column: x => x.NBestsId,
                        principalTable: "NBests",
                        principalColumn: "Id");
                });

            migrationBuilder.CreateIndex(
                name: "IX_CombinedRecognizedPhrases_TranscriptionsId",
                table: "CombinedRecognizedPhrases",
                column: "TranscriptionsId");

            migrationBuilder.CreateIndex(
                name: "IX_NBests_RecognizedPhrasesId",
                table: "NBests",
                column: "RecognizedPhrasesId");

            migrationBuilder.CreateIndex(
                name: "IX_RecognizedPhrases_CombinedRecognizedPhrasesId",
                table: "RecognizedPhrases",
                column: "CombinedRecognizedPhrasesId");

            migrationBuilder.CreateIndex(
                name: "IX_Words_NBestsId",
                table: "Words",
                column: "NBestsId");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            _ = migrationBuilder ?? throw new ArgumentNullException(nameof(migrationBuilder));

            migrationBuilder.DropTable(
                name: "Words");

            migrationBuilder.DropTable(
                name: "NBests");

            migrationBuilder.DropTable(
                name: "RecognizedPhrases");

            migrationBuilder.DropTable(
                name: "CombinedRecognizedPhrases");

            migrationBuilder.DropTable(
                name: "Transcriptions");
        }
    }
}
